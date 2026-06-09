"""Generate architecture and flowchart diagrams for TCC."""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import matplotlib.patheffects as pe
import numpy as np

OUT = '/home/rbabynho/workspace/Command_Workstation/docs/tcc_assets/'

# ─────────────────────────────────────────────────────────────
# Diagram 1: Component Architecture
# ─────────────────────────────────────────────────────────────

def draw_box(ax, x, y, w, h, label, sublabel='', color='#2d6cdf', text_color='white',
             fontsize=11, sublabel_size=8.5, radius=0.04):
    box = FancyBboxPatch((x - w/2, y - h/2), w, h,
                         boxstyle=f"round,pad=0.01,rounding_size={radius}",
                         facecolor=color, edgecolor='#1a1a2e', linewidth=1.4,
                         zorder=3)
    ax.add_patch(box)
    if sublabel:
        ax.text(x, y + 0.06, label, ha='center', va='center',
                fontsize=fontsize, color=text_color, fontweight='bold', zorder=4)
        ax.text(x, y - 0.10, sublabel, ha='center', va='center',
                fontsize=sublabel_size, color=text_color, alpha=0.85, zorder=4,
                style='italic')
    else:
        ax.text(x, y, label, ha='center', va='center',
                fontsize=fontsize, color=text_color, fontweight='bold', zorder=4)

def arrow(ax, x1, y1, x2, y2, color='#555', lw=1.5, style='->', label=''):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle=style, color=color,
                                lw=lw, connectionstyle='arc3,rad=0'))
    if label:
        mx, my = (x1+x2)/2, (y1+y2)/2
        ax.text(mx+0.02, my, label, fontsize=7.5, color='#444', ha='left', va='center', zorder=5)

fig, ax = plt.subplots(figsize=(13, 8))
ax.set_xlim(0, 13)
ax.set_ylim(0, 8)
ax.axis('off')
fig.patch.set_facecolor('#f8f9fa')
ax.set_facecolor('#f8f9fa')

# Title
# Título removido da imagem — vai na legenda abaixo da figura no documento Word

# Layer backgrounds
def layer_bg(ax, y_bottom, height, label, color):
    rect = plt.Rectangle((0.3, y_bottom), 12.4, height,
                          facecolor=color, edgecolor='#ccc', linewidth=1,
                          linestyle='--', zorder=1, alpha=0.45)
    ax.add_patch(rect)
    ax.text(0.55, y_bottom + height/2, label, fontsize=8, color='#555',
            va='center', rotation=90, fontweight='bold')

layer_bg(ax, 5.7, 1.5, 'Camada de\nApresentação', '#e8f4fd')
layer_bg(ax, 3.5, 2.0, 'Camada de\nOrquestração', '#e8fdf0')
layer_bg(ax, 1.3, 2.0, 'Camada de\nInfraestrutura', '#fff8e1')
layer_bg(ax, 0.1, 1.0, 'Persistência', '#fce4ec')

# Presentation layer: Main.qml
draw_box(ax, 6.5, 6.45, 3.2, 0.75, 'Main.qml', 'Interface QML / Qt Quick',
         color='#1565c0')

# Orchestration layer
draw_box(ax, 6.5, 4.5, 3.2, 0.75, 'WorkstationBridge', 'Ponte C++ ↔ QML (Q_INVOKABLE)',
         color='#2e7d32')

# Infrastructure layer
draw_box(ax, 2.8, 2.45, 2.4, 0.75, 'CommandRunner', 'QProcess — subprocessos assíncronos',
         color='#e65100')
draw_box(ax, 6.5, 2.45, 2.4, 0.75, 'TerminalLauncher', 'TTY interativo do sistema',
         color='#6a1b9a')
draw_box(ax, 10.2, 2.45, 2.4, 0.75, 'DatabaseManager', 'Qt SQL / SQLite',
         color='#37474f')

# Persistence layer
draw_box(ax, 6.5, 0.62, 4.0, 0.65, 'command_workstation.sqlite',
         'settings · presets · run_log',
         color='#880e4f', fontsize=10)

# External box: OS / System
draw_box(ax, 2.8, 1.5, 2.2, 0.5, 'Sistema Operacional', '(processos, sinais)',
         color='#78909c', fontsize=9)

# Arrows: QML → Bridge
arrow(ax, 6.5, 6.08, 6.5, 4.88, color='#1565c0', lw=2, label='sinais / invocações')

# Bridge → CommandRunner
arrow(ax, 5.1, 4.14, 4.0, 2.83, color='#2e7d32', lw=1.8, label='runCommand()')
# Bridge → TerminalLauncher
arrow(ax, 6.5, 4.12, 6.5, 2.83, color='#2e7d32', lw=1.8, label='launchTerminal()')
# Bridge → DatabaseManager
arrow(ax, 7.9, 4.14, 9.0, 2.83, color='#2e7d32', lw=1.8)
ax.text(8.65, 3.65, 'CRUD presets\n/ run_log', fontsize=7.5, color='#444', ha='center')

# CommandRunner → OS
arrow(ax, 2.8, 2.08, 2.8, 1.75, color='#e65100', lw=1.5, label='QProcess')
# CommandRunner → Bridge (output)
ax.annotate('', xy=(5.1, 4.14), xytext=(3.8, 2.83),
            arrowprops=dict(arrowstyle='->', color='#b71c1c', lw=1.4, linestyle='dashed',
                            connectionstyle='arc3,rad=-0.3'))
ax.text(3.9, 3.5, 'outputLine()', fontsize=7.5, color='#b71c1c', ha='center',
        style='italic')

# DatabaseManager → SQLite
arrow(ax, 10.2, 2.08, 8.5, 0.95, color='#37474f', lw=1.8)

# Legend
legend_items = [
    mpatches.Patch(color='#1565c0', label='Apresentação (QML)'),
    mpatches.Patch(color='#2e7d32', label='Orquestração (C++)'),
    mpatches.Patch(color='#e65100', label='Execução de comandos'),
    mpatches.Patch(color='#6a1b9a', label='Terminal interativo'),
    mpatches.Patch(color='#37474f', label='Persistência (SQLite)'),
]
ax.legend(handles=legend_items, loc='lower left', fontsize=8, framealpha=0.85,
          bbox_to_anchor=(0.02, 0.01))

plt.tight_layout(pad=0.3)
plt.savefig(OUT + 'diagrama_arquitetura.png', dpi=180, bbox_inches='tight',
            facecolor='#f8f9fa')
plt.close()
print('Diagram 1 saved.')

# ─────────────────────────────────────────────────────────────
# Diagram 2: Command Execution Flowchart
# ─────────────────────────────────────────────────────────────

fig2, ax2 = plt.subplots(figsize=(13, 7.5))
ax2.set_xlim(0, 10)
ax2.set_ylim(0, 9.5)
ax2.axis('off')
fig2.patch.set_facecolor('#f8f9fa')

# Título removido da imagem — vai na legenda abaixo da figura no documento Word

def flow_rect(ax, cx, cy, w, h, text, color='#1565c0', tc='white', fs=10):
    box = FancyBboxPatch((cx-w/2, cy-h/2), w, h,
                         boxstyle='round,pad=0.015,rounding_size=0.06',
                         facecolor=color, edgecolor='#333', lw=1.2, zorder=3)
    ax.add_patch(box)
    ax.text(cx, cy, text, ha='center', va='center', fontsize=fs,
            color=tc, fontweight='bold', zorder=4, wrap=True,
            multialignment='center')

def flow_diamond(ax, cx, cy, w, h, text, color='#f57c00', tc='white', fs=9):
    dx, dy = w/2, h/2
    pts = np.array([[cx, cy+dy], [cx+dx, cy], [cx, cy-dy], [cx-dx, cy]])
    poly = plt.Polygon(pts, closed=True, facecolor=color,
                       edgecolor='#333', lw=1.2, zorder=3)
    ax.add_patch(poly)
    ax.text(cx, cy, text, ha='center', va='center', fontsize=fs,
            color=tc, fontweight='bold', zorder=4, multialignment='center')

def farrow(ax, x1, y1, x2, y2, label='', lc='#444'):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle='->', color=lc, lw=1.6))
    if label:
        ax.text((x1+x2)/2+0.08, (y1+y2)/2, label, fontsize=8, color=lc)

left_x, right_x = 2.8, 7.2

y_top = 8.4
y_step = 1.1

# Coluna esquerda: fluxo principal
flow_rect(ax2, left_x, y_top, 2.9, 0.58, 'Usuário seleciona\npasta de trabalho', '#455a64', fs=9)
farrow(ax2, left_x, y_top-0.29, left_x, y_top-y_step+0.29)

flow_diamond(ax2, left_x, y_top-y_step, 2.8, 0.68, 'É repositório\nGit?', '#f57c00')
ax2.text(left_x+0.35, y_top-y_step+0.35, 'Sim', fontsize=8, color='#2e7d32')
ax2.text(left_x+0.35, y_top-y_step-0.35, 'Não', fontsize=8, color='#c62828')
farrow(ax2, left_x, y_top-y_step-0.34, left_x, y_top-2*y_step+0.34)

flow_rect(ax2, left_x, y_top-2*y_step, 2.9, 0.58, 'Usuário clica em\npreset ou digita comando', '#1565c0', fs=9)
farrow(ax2, left_x, y_top-2*y_step-0.29, left_x, y_top-3*y_step+0.29)

flow_diamond(ax2, left_x, y_top-3*y_step, 2.8, 0.68, '"System terminal"\nativado?', '#f57c00')
farrow(ax2, left_x, y_top-3*y_step-0.34, left_x, y_top-4*y_step+0.34)

flow_rect(ax2, left_x, y_top-4*y_step, 2.9, 0.58, 'WorkstationBridge\nrunShellCommand()', '#2e7d32', fs=9)
farrow(ax2, left_x, y_top-4*y_step-0.29, left_x, y_top-5*y_step+0.29)

flow_rect(ax2, left_x, y_top-5*y_step, 2.9, 0.58, 'Processo encerra\n(exitCode)', '#455a64', fs=9)
farrow(ax2, left_x, y_top-5*y_step-0.29, left_x, y_top-6*y_step+0.29)

flow_rect(ax2, left_x, y_top-6*y_step, 2.4, 0.58, 'FIM', '#1a1a2e', fs=11)

# Coluna direita: respostas e execução
flow_rect(ax2, right_x, y_top, 2.9, 0.58, 'Exibe branch\n+ status Git', '#1b5e20', fs=9)
farrow(ax2, right_x, y_top-0.29, right_x, y_top-y_step+0.29)

flow_rect(ax2, right_x, y_top-y_step, 2.9, 0.58, 'TerminalLauncher\nabrir TTY', '#6a1b9a', fs=9)
farrow(ax2, right_x, y_top-y_step-0.29, right_x, y_top-2*y_step+0.29)

flow_rect(ax2, right_x, y_top-2*y_step, 2.9, 0.58, 'CommandRunner\nQProcess::start()', '#e65100', fs=9)
farrow(ax2, right_x, y_top-2*y_step-0.29, right_x, y_top-3*y_step+0.29)

flow_rect(ax2, right_x, y_top-3*y_step, 2.9, 0.58, 'stdout / stderr\nem tempo real', '#37474f', fs=9)
farrow(ax2, right_x, y_top-3*y_step-0.29, right_x, y_top-4*y_step+0.29)

flow_rect(ax2, right_x, y_top-4*y_step, 2.9, 0.58, 'outputLine(text, isErr)\n→ Terminal QML', '#1565c0', fs=9)
farrow(ax2, right_x, y_top-4*y_step-0.29, right_x, y_top-5*y_step+0.29)

flow_rect(ax2, right_x, y_top-5*y_step, 3.0, 0.58, 'DatabaseManager\nINSERT em run_log', '#880e4f', fs=9)
ax2.text(right_x, y_top-5*y_step-0.28, '(comando, cwd, exitCode, duration_ms, output)', fontsize=7.2,
         color='#555', ha='center')
farrow(ax2, right_x, y_top-5*y_step-0.58, right_x, y_top-6*y_step+0.29)

flow_rect(ax2, right_x, y_top-6*y_step, 3.0, 0.58, 'UI atualizada:\nRecent Runs recarregado', '#1565c0', fs=9)

# Conexões entre as colunas
ax2.annotate('', xy=(right_x-0.8, y_top), xytext=(left_x+1.1, y_top),
             arrowprops=dict(arrowstyle='->', color='#444', lw=1.4))
ax2.text((left_x+right_x)/2+0.15, y_top+0.13, 'Sim', fontsize=8, color='#2e7d32')

ax2.annotate('', xy=(right_x-0.7, y_top-y_step), xytext=(left_x+1.2, y_top-3*y_step),
             arrowprops=dict(arrowstyle='->', color='#444', lw=1.4))
ax2.text((left_x+right_x)/2+0.05, y_top-y_step+0.05, 'Sim', fontsize=8, color='#2e7d32')

ax2.annotate('', xy=(right_x-0.6, y_top-2*y_step), xytext=(left_x+1.2, y_top-4*y_step),
             arrowprops=dict(arrowstyle='->', color='#444', lw=1.4))

# Stop button side annotation
ax2.annotate('Usuário pode\nclicar Stop\n(SIGTERM)',
             xy=(3.1, y_top-4*y_step), xytext=(0.9, y_top-3.2*y_step),
             fontsize=8, color='#b71c1c',
             arrowprops=dict(arrowstyle='->', color='#b71c1c', lw=1.2),
             bbox=dict(boxstyle='round', facecolor='#ffebee', edgecolor='#b71c1c', alpha=0.9))

plt.tight_layout(pad=0.3)
plt.savefig(OUT + 'fluxograma_execucao.png', dpi=180, bbox_inches='tight',
            facecolor='#f8f9fa')
plt.close()
print('Diagram 2 saved.')

# ─────────────────────────────────────────────────────────────
# Diagram 3: Database schema
# ─────────────────────────────────────────────────────────────

fig3, ax3 = plt.subplots(figsize=(12, 5))
ax3.set_xlim(0, 12)
ax3.set_ylim(0, 5)
ax3.axis('off')
fig3.patch.set_facecolor('#f8f9fa')

# Título removido da imagem — vai na legenda abaixo da figura no documento Word

def db_table(ax, x, y, title, fields, color='#1565c0', width=3.2):
    row_h = 0.38
    total_h = row_h * (len(fields) + 1)
    # Header
    hdr = FancyBboxPatch((x, y - row_h), width, row_h,
                         boxstyle='round,pad=0.01,rounding_size=0.03',
                         facecolor=color, edgecolor='#333', lw=1.3, zorder=3)
    ax.add_patch(hdr)
    ax.text(x + width/2, y - row_h/2, title,
            ha='center', va='center', fontsize=10,
            color='white', fontweight='bold', zorder=4)
    # Rows
    for i, (fname, ftype, pk) in enumerate(fields):
        ry = y - row_h * (i + 2)
        bg = '#e3f2fd' if i % 2 == 0 else '#f8f9fa'
        row_rect = plt.Rectangle((x, ry), width, row_h,
                                  facecolor=bg, edgecolor='#bbb', lw=0.8, zorder=2)
        ax.add_patch(row_rect)
        prefix = 'PK ' if pk else '   '
        ax.text(x + 0.12, ry + row_h/2, prefix + fname,
                va='center', fontsize=8.5, color='#1a1a2e', zorder=4)
        ax.text(x + width - 0.12, ry + row_h/2, ftype,
                va='center', ha='right', fontsize=8, color='#555',
                style='italic', zorder=4)
    return total_h

# Table: presets
db_table(ax3, 0.4, 4.3, 'presets', [
    ('id', 'INTEGER PK', True),
    ('name', 'TEXT', False),
    ('command', 'TEXT', False),
    ('category', 'TEXT', False),
    ('cwd', 'TEXT', False),
    ('button_color', 'TEXT', False),
    ('is_builtin', 'INTEGER', False),
], color='#1565c0')

# Table: run_log
db_table(ax3, 4.4, 4.3, 'run_log', [
    ('id', 'INTEGER PK', True),
    ('command', 'TEXT', False),
    ('cwd', 'TEXT', False),
    ('started_at', 'TEXT', False),
    ('exit_code', 'INTEGER', False),
    ('duration_ms', 'INTEGER', False),
    ('output_excerpt', 'TEXT', False),
], color='#880e4f')

# Table: settings
db_table(ax3, 8.4, 4.3, 'settings', [
    ('key', 'TEXT PK', True),
    ('value', 'TEXT', False),
], color='#37474f', width=3.2)

plt.tight_layout(pad=0.3)
plt.savefig(OUT + 'schema_banco.png', dpi=180, bbox_inches='tight',
            facecolor='#f8f9fa')
plt.close()
print('Diagram 3 saved.')
print('All diagrams generated in', OUT)
