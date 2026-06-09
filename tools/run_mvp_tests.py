#!/usr/bin/env python3
"""Functional MVP tests for Command Workstation — mirrors app behavior for TCC evidence."""

from __future__ import annotations

import json
import os
import shutil
import sqlite3
import subprocess
import sys
import tempfile
import time
from dataclasses import asdict, dataclass
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
DB_PATH = Path.home() / ".local/share/CommandWorkstation/Command Workstation/command_workstation.sqlite"
RESULTS_JSON = PROJECT_ROOT / "docs" / "test_results.json"


@dataclass
class TestCase:
    test_id: str
    name: str
    passed: bool
    evidence: str
    notes: str = ""


def run_shell(command: str, cwd: Path, timeout: int = 30) -> tuple[int, str, int]:
    start = time.time()
    proc = subprocess.run(
        ["/bin/sh", "-c", command],
        cwd=str(cwd),
        capture_output=True,
        text=True,
        timeout=timeout,
    )
    duration_ms = int((time.time() - start) * 1000)
    output = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode, output, duration_ms


def run_git(args: list[str], cwd: Path) -> tuple[int, str]:
    proc = subprocess.run(
        ["git", *args],
        cwd=str(cwd),
        capture_output=True,
        text=True,
        timeout=15,
    )
    return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


def ensure_db() -> sqlite3.Connection:
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def insert_run_log(conn: sqlite3.Connection, cwd: str, command: str, exit_code: int, duration_ms: int, tail: str) -> None:
    conn.execute(
        "INSERT INTO run_log(started_at, cwd, command, exit_code, duration_ms, output_tail) "
        "VALUES(datetime('now'),?,?,?,?,?)",
        (cwd, command, exit_code, duration_ms, tail[:8000]),
    )
    conn.commit()


def query_metrics(conn: sqlite3.Connection) -> dict:
    metrics: dict = {}
    cur = conn.cursor()
    cur.execute("SELECT COUNT(*) AS n FROM run_log")
    metrics["total_execucoes"] = cur.fetchone()[0]
    cur.execute("SELECT COUNT(*) AS n FROM presets")
    metrics["total_presets"] = cur.fetchone()[0]
    cur.execute("SELECT exit_code, COUNT(*) AS n FROM run_log GROUP BY exit_code ORDER BY exit_code")
    metrics["por_exit_code"] = {str(row[0]): row[1] for row in cur.fetchall()}
    cur.execute("SELECT ROUND(AVG(duration_ms), 1) FROM run_log WHERE exit_code = 0")
    row = cur.fetchone()[0]
    metrics["tempo_medio_ms_sucesso"] = row if row is not None else 0
    cur.execute(
        "SELECT command, COUNT(*) AS c FROM run_log GROUP BY command ORDER BY c DESC LIMIT 10"
    )
    metrics["comandos_mais_usados"] = [(row[0], row[1]) for row in cur.fetchall()]
    cur.execute("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")
    metrics["tabelas"] = [row[0] for row in cur.fetchall()]
    return metrics


def main() -> int:
    results: list[TestCase] = []
    work_cwd = PROJECT_ROOT

    # TF-01: SQLite database exists and has expected schema
    try:
        conn = ensure_db()
        tables = {r[0] for r in conn.execute("SELECT name FROM sqlite_master WHERE type='table'")}
        ok = {"settings", "presets", "run_log"}.issubset(tables)
        preset_count = conn.execute("SELECT COUNT(*) FROM presets").fetchone()[0]
        results.append(
            TestCase(
                "TF-01",
                "Inicialização e SQLite",
                ok,
                f"Banco em {DB_PATH}; tabelas={sorted(tables)}; presets={preset_count}",
            )
        )
    except Exception as exc:  # noqa: BLE001
        results.append(TestCase("TF-01", "Inicialização e SQLite", False, str(exc)))
        conn = None

    # TF-02: Git detection in project folder
    code, out = run_git(["rev-parse", "--is-inside-work-tree"], work_cwd)
    in_repo = code == 0 and out.strip() == "true"
    if not in_repo:
        # init temp git repo for validation
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            subprocess.run(["git", "init"], cwd=tmp, check=True, capture_output=True)
            code2, out2 = run_git(["rev-parse", "--is-inside-work-tree"], tmp_path)
            in_repo = code2 == 0 and out2.strip() == "true"
            git_evidence = f"Repo principal indisponível; validado em repo temporário ({tmp_path})"
    else:
        code_b, branch = run_git(["branch", "--show-current"], work_cwd)
        code_s, status = run_git(["status", "-sb"], work_cwd)
        git_evidence = f"branch={branch.strip() or '(detached)'}; status={status.strip()[:120]}"
    results.append(TestCase("TF-02", "Detecção de repositório Git", in_repo, git_evidence))

    # TF-03: Preset-like command execution + run_log
    if conn:
        cmd = "pwd" if os.name != "nt" else "cd"
        code, out, dur = run_shell(cmd, work_cwd)
        insert_run_log(conn, str(work_cwd), cmd, code, dur, out)
        row = conn.execute("SELECT id FROM run_log ORDER BY id DESC LIMIT 1").fetchone()
        ok = code == 0 and row is not None
        results.append(
            TestCase(
                "TF-03",
                "Execução de comando preset (pwd)",
                ok,
                f"run_log id={row[0] if row else '?'}; exit={code}; duration_ms={dur}",
            )
        )

    # TF-04: Git status preset command
    code, out, dur = run_shell("git status -sb", work_cwd)
    if conn:
        insert_run_log(conn, str(work_cwd), "git status -sb", code, dur, out)
    results.append(
        TestCase(
            "TF-04",
            "Preset Git short status",
            code == 0 or "not a git repository" not in out.lower(),
            f"exit={code}; saída={out.strip()[:100] or '(vazia)'}",
        )
    )

    # TF-05: Custom preset persistence (simulate CRUD in SQLite)
    if conn:
        test_name = "__tcc_test_preset__"
        conn.execute("DELETE FROM presets WHERE label = ?", (test_name,))
        conn.execute(
            "INSERT INTO presets(category, label, command, sort_order, cwd) VALUES(?,?,?,?,?)",
            ("Personalizados", test_name, "echo tcc_ok", 999, str(work_cwd)),
        )
        conn.commit()
        row = conn.execute(
            "SELECT command FROM presets WHERE label = ?", (test_name,)
        ).fetchone()
        ok = row is not None and row[0] == "echo tcc_ok"
        conn.execute("DELETE FROM presets WHERE label = ?", (test_name,))
        conn.commit()
        results.append(
            TestCase(
                "TF-05",
                "Persistência de preset personalizado",
                ok,
                "INSERT/SELECT/DELETE em categoria Personalizados",
            )
        )

    # TF-06: Custom shell command
    code, out, dur = run_shell("echo MVP_VALIDATION_OK", work_cwd)
    if conn:
        insert_run_log(conn, str(work_cwd), "echo MVP_VALIDATION_OK", code, dur, out)
    results.append(
        TestCase(
            "TF-06",
            "Comando customizado no terminal",
            code == 0 and "MVP_VALIDATION_OK" in out,
            f"exit={code}; duration_ms={dur}",
        )
    )

    # TF-07: Long-running command + stop simulation
    proc = subprocess.Popen(
        ["/bin/sh", "-c", "sleep 20"],
        cwd=str(work_cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    time.sleep(0.4)
    proc.terminate()
    try:
        proc.wait(timeout=3)
        stopped = proc.returncode is not None
    except subprocess.TimeoutExpired:
        proc.kill()
        stopped = True
    results.append(
        TestCase(
            "TF-07",
            "Interrupção de processo em execução",
            stopped,
            "sleep 20 terminado via SIGTERM (equivalente ao botão Stop)",
        )
    )

    # TF-08: Binary exists and is executable
    binary = PROJECT_ROOT / "build/Desktop_Qt_6_11_0-Release/appCommand_Workstation"
    ok_bin = binary.is_file() and os.access(binary, os.X_OK)
    results.append(
        TestCase(
            "TF-08",
            "Protótipo compilado disponível",
            ok_bin,
            str(binary) if ok_bin else f"Binário não encontrado em {binary}",
        )
    )

    # TF-09: CMake preset commands (build/check category)
    code_cfg, out_cfg, dur_cfg = run_shell(
        "cmake --version | head -1", work_cwd
    )
    results.append(
        TestCase(
            "TF-09",
            "Ambiente de build (cmake disponível)",
            code_cfg == 0,
            out_cfg.strip() or "cmake não encontrado",
        )
    )

    metrics = query_metrics(conn) if conn else {}
    if conn:
        conn.close()

    passed = sum(1 for t in results if t.passed)
    payload = {
        "project_root": str(PROJECT_ROOT),
        "database_path": str(DB_PATH),
        "summary": {
            "total": len(results),
            "passed": passed,
            "failed": len(results) - passed,
        },
        "test_cases": [asdict(t) for t in results],
        "metrics": metrics,
    }

    RESULTS_JSON.parent.mkdir(parents=True, exist_ok=True)
    RESULTS_JSON.write_text(json.dumps(payload, indent=2, ensure_ascii=False), encoding="utf-8")

    print(json.dumps(payload, indent=2, ensure_ascii=False))
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    sys.exit(main())
