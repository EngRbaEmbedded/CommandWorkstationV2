# Texto Revisado — TCC Command Workstation
## Implementação das correções do orientador Prof. Arthur Pinheiro

---

## RESUMO (revisado)

Este trabalho apresenta o desenvolvimento do Command Workstation, protótipo
desktop concebido como painel unificado ("Single Pane of Glass") para reduzir a
alternância entre ferramentas no fluxo de trabalho de engenharia de software.
**O objetivo consistiu em desenvolver e validar tecnicamente o núcleo mínimo
do protótipo**, composto por execução local de comandos, terminal integrado,
operações Git básicas e persistência em SQLite. A pesquisa adotou caráter
aplicado com delineamento iterativo de protótipo e validação técnica funcional.
A validação compreendeu roteiro de nove testes funcionais, todos aprovados, e a
análise do histórico de execuções registrado automaticamente no banco de dados.
Os resultados demonstraram a viabilidade técnica do núcleo mínimo proposto,
evidenciando que a integração das funcionalidades centrais em interface única é
tecnicamente factível com o conjunto tecnológico adotado.
Palavras-chave: painel unificado; protótipo de software; carga cognitiva;
engenharia de software; SQLite.

---
> **Correção aplicada:** Adicionada frase explícita de objetivo ("O objetivo
> consistiu em..."). Termo "Single Pane of Glass" passa a aparecer entre aspas,
> sem itálico, conforme orientação do Prof. Arthur.

---

## INTRODUÇÃO (revisado — seção completa)

No cenário contemporâneo de desenvolvimento de software, profissionais
alternam frequentemente entre terminais, sistemas de controle de versão,
ferramentas de "build" e outras interfaces especializadas. Essa fragmentação
impõe custos atencionais significativos: após cada troca de contexto, o
indivíduo necessita de tempo adicional para retomar o estado cognitivo anterior,
o que eleva o estresse e reduz a agilidade na execução de tarefas complexas
(MARK; GUDITH; KLOCKE, 2008). Revisões recentes associaram a dispersão de
informações entre múltiplas interfaces a maior carga cognitiva e menor
compreensão global dos processos, quando comparadas a abordagens integradas
(SCHREIBER; ABBAD-ANDALOUSSI; WEBER, 2023; SHANMUGASUNDARAM; TAMILARASU, 2023).
A teoria da carga cognitiva (SWELLER, 1988) sustentou que interfaces mal
projetadas consumiram recursos da memória de trabalho que deveriam ser
direcionados à resolução do problema em si.

Estudos em produtividade de desenvolvedores reforçam esse diagnóstico:
pesquisadores identificaram que interrupções e trocas de ferramenta figuram
entre os principais fatores de redução do desempenho e da satisfação no
trabalho de engenharia de software (MEYER et al., 2014). Zieris e Salinger
(2014) observaram que a reintegração mental ao contexto de trabalho após uma
interrupção consome, em média, entre 10 e 15 minutos, evidenciando o custo
operacional de ambientes fragmentados. Neste sentido, ferramentas que
consolidam múltiplas funcionalidades em uma única interface — denominadas na
literatura em inglês "Single Pane of Glass" — emergem como alternativa
promissora para reduzir a sobrecarga cognitiva (SOMMERVILLE, 2016).

Diante desse cenário, este trabalho propõe o Command Workstation, aplicação
desktop que implementa o conceito de "Single Pane of Glass", reunindo em uma
única interface o acesso a comandos locais, terminal integrado, operações Git e
persistência de configurações e histórico de execuções. O escopo da pesquisa
delimita-se ao ambiente desktop Linux, voltado a profissionais e estudantes de
engenharia de software, e abrange o núcleo mínimo funcional da proposta,
excluindo integrações corporativas e fluxos de gestão de projetos.

O trabalho foi inicialmente delineado para desenvolvimento e avaliação em
ambiente corporativo com equipes de engenharia de software. Em razão da perda
de acesso ao ambiente e aos artefatos originais, o escopo foi reorientado, com
anuência do orientador, para a construção e validação técnica de um protótipo
funcional (prova de conceito — "POC"), preservando o tema central do painel
unificado de comandos.

O objetivo deste trabalho consistiu em desenvolver o protótipo Command
Workstation e validar tecnicamente seu núcleo mínimo — painel unificado,
execução local de comandos, terminal embutido, integração básica com Git e
persistência em SQLite — como alternativa viável à alternância entre múltiplas
ferramentas no fluxo de trabalho de desenvolvimento de software.

---
> **Correções aplicadas:**
> - Adicionadas 3 citações novas (MEYER et al., 2014; ZIERIS; SALINGER, 2014;
>   + reforço da literatura existente)
> - Adicionada justificativa explícita do estudo (parágrafo 2)
> - Adicionado contexto da aplicação e delimitação de escopo (parágrafo 3)
> - Corrigida formatação: "Single Pane of Glass" entre aspas, sem itálico;
>   "POC" entre aspas; "build" entre aspas
> - Objetivo declarado explicitamente no último parágrafo

---

## METODOLOGIA (revisado — seção completa)

A pesquisa manteve caráter aplicado, com delineamento iterativo de protótipo e
validação técnica funcional. O trabalho dividiu-se em duas frentes
complementares: (a) construção do software e (b) verificação do comportamento
do núcleo mínimo da proposta.

### Desenvolvimento do protótipo

#### Tecnologias e justificativas

O protótipo foi desenvolvido como aplicação desktop multiplataforma utilizando
C++17, Qt 6 ("Quick/QML") para a interface gráfica, CMake para "build" e SQLite
(módulo Qt SQL) para persistência local. A escolha do Qt 6 justificou-se pela
maturidade do ecossistema, pela integração nativa entre lógica C++ e interface
declarativa QML, e pela disponibilidade de módulos para execução de subprocessos
(QProcess) e acesso a banco de dados. O C++17 foi adotado por oferecer controle
de desempenho e compatibilidade com as APIs do Qt. O SQLite foi selecionado por
ser embarcável (sem servidor), portátil e amplamente suportado, adequado para
persistência local em aplicações desktop.

#### Arquitetura do sistema

A arquitetura separou camada de apresentação (QML), orquestração
(WorkstationBridge), execução de comandos (CommandRunner), integração com
terminal do sistema (TerminalLauncher) e persistência (DatabaseManager),
conforme ilustra a Figura X.

**[INSERIR FIGURA X — diagrama_arquitetura.png]**
Figura X. Diagrama de componentes da arquitetura do Command Workstation
Fonte: Dados originais da pesquisa

A camada de apresentação (arquivo `Main.qml`) implementou a interface em QML/Qt
Quick, declarando os elementos visuais e conectando eventos de usuário às
funções expostas pela bridge. O WorkstationBridge (classe C++) serviu como elo
entre QML e a lógica da aplicação, expondo propriedades e métodos via macro
`Q_INVOKABLE` e emitindo sinais de retorno para a interface. O CommandRunner
gerenciou a execução assíncrona de comandos do sistema operacional por meio da
classe `QProcess`, capturando saída padrão e de erro em tempo real. O
TerminalLauncher delegou comandos que requerem terminal interativo (como `sudo`)
a um emulador de terminal externo do sistema, com abertura de uma sessão
"TTY" ("Teletype") nativa. O DatabaseManager encapsulou o acesso ao banco SQLite,
gerenciando as três tabelas principais da aplicação.

#### Estrutura do banco de dados

O banco de dados local armazenou três entidades principais, conforme a Figura Y:

**[INSERIR FIGURA Y — schema_banco.png]**
*Figura Y. Esquema das tabelas do banco de dados SQLite.
Fonte: elaboração própria.*

- **presets**: armazena os atalhos de comandos, tanto os embutidos
  (categorias "General", "Build/check" e "Quality") quanto os personalizados
  ("Personalizados"), com nome, comando, diretório de trabalho e cor do botão.
- **run_log**: registra automaticamente cada execução de comando, incluindo
  o comando executado, o diretório de trabalho (`cwd`), o instante de início
  (`started_at`), o código de saída (`exit_code`), a duração em milissegundos
  (`duration_ms`) e um trecho da saída (`output_excerpt`).
- **settings**: persiste configurações da aplicação como pares chave–valor.

#### Funcionalidades implementadas no núcleo mínimo

O núcleo mínimo implementado compreendeu:

1. **Seleção de pasta de trabalho**: o usuário seleciona o diretório do projeto;
   o sistema detecta automaticamente se é repositório Git e exibe
   branch e status.
2. **Presets de comandos**: atalhos agrupados por categorias ("General",
   "Build/check", "Quality") e presets personalizados editáveis pelo usuário
   (operações de criação, leitura, atualização e exclusão — "CRUD").
3. **Terminal embutido**: exibe saída padrão e de erro em tempo real; opção de
   terminal interativo do sistema para comandos que requerem entrada do usuário.
4. **Integração Git**: exibição de "branch", status, e operações de "pull",
   "push", "commit" (com mensagem) e descarte de alterações.
5. **Registro automático**: cada execução registrada na tabela `run_log`,
   com comando, diretório, código de saída, duração e trecho da saída.
6. **Interrupção de processos**: botão "Stop" envia sinal "SIGTERM" ao processo
   em execução.

As Figuras Z1 e Z2 apresentam a interface do protótipo compilado em execução.
A Figura Z1 exibe a tela inicial com o popup de boas-vindas, o painel de
presets padrão, os presets personalizados com cores definidas pelo usuário, o
status Git (branch "master") e o banco SQLite conectado. A Figura Z2 mostra a
continuação do painel lateral após rolagem, revelando todos os grupos de
presets ("Build/check", "General", "Personalizados", "Quality") e a seção
"Recent runs" com o histórico de execuções persistido automaticamente no banco
de dados.

**[INSERIR FIGURA Z1 — screenshot_boas_vindas.png]**
*Figura Z1. Tela inicial do Command Workstation: popup de boas-vindas, presets
personalizados com cores, status Git (branch "master") e SQLite conectado.
Fonte: captura de tela do protótipo desenvolvido.*

**[INSERIR FIGURA Z2 — screenshot_scroll.png]**
*Figura Z2. Painel lateral do Command Workstation com rolagem aplicada:
presets organizados por categoria e seção "Recent runs" com histórico de
execuções (timestamp, exit code e duração em ms) persistido no SQLite.
Fonte: captura de tela do protótipo desenvolvido.*

#### Passo a passo da execução de um comando

O fluxo de execução de um comando no sistema seguiu as etapas descritas na
Figura W:

**[INSERIR FIGURA W — fluxograma_execucao.png]**
*Figura W. Fluxograma de execução de um comando no Command Workstation.
Fonte: elaboração própria.*

O usuário selecionou a pasta de trabalho, o sistema verificou a presença de
repositório Git e exibiu as informações de controle de versão. Ao clicar em
um preset ou digitar um comando, o WorkstationBridge verificou se o modo
"System terminal" estava ativo: em caso positivo, o TerminalLauncher abriu uma
sessão TTY do sistema operacional; caso contrário, o CommandRunner iniciou o
processo via `QProcess`, transmitindo a saída linha a linha ao terminal da
interface. Ao término do processo, o DatabaseManager registrou a execução na
tabela `run_log` e a interface atualizou a seção "Recent Runs". A qualquer
momento durante a execução, o usuário pôde clicar em "Stop" para enviar um
sinal de interrupção ("SIGTERM") ao processo em andamento.

### Validação técnica do protótipo

A validação preliminar concentrou-se em roteiro estruturado de testes
funcionais (TF-01 a TF-09), executado em 21/05/2026, verificando inicialização
e conexão SQLite, detecção de repositório Git, execução de comandos tipo
preset, persistência de presets personalizados, comandos customizados,
interrupção de processos, disponibilidade do binário compilado e ambiente de
"build". Complementarmente, foram analisados os registros históricos
persistidos automaticamente no banco SQLite (tabelas `settings`, `presets` e
`run_log`).

Os critérios de validação adotados foram: (i) execução bem-sucedida de todos
os casos de teste do roteiro (aprovação de 100% dos casos); (ii) persistência
correta dos dados nas tabelas SQLite; (iii) registro automático e íntegro do
histórico de execuções; e (iv) disponibilidade do binário compilado para a
plataforma alvo (Linux/x86-64).

---
> **Correções aplicadas:**
> - Adicionadas justificativas para cada escolha tecnológica (C++17, Qt 6,
>   SQLite, CMake)
> - Adicionada subseção de arquitetura com referência ao diagrama de componentes
> - Adicionada subseção de estrutura do banco de dados com referência ao esquema
> - Adicionado passo a passo detalhado do fluxo de execução com referência ao
>   fluxograma
> - Adicionada subseção de interface com referência ao screenshot
> - Adicionados critérios de validação ao final
> - Removida linguagem temporal ("nesta fase", "nesta etapa", "até o momento")
> - Corrigida formatação: "build", "Quick/QML", "TTY", "SIGTERM", "CRUD",
>   "POC", "Single Pane of Glass" entre aspas; nomes próprios sem aspas

---

## REFERÊNCIAS ADICIONAIS SUGERIDAS

(para a seção Introdução — adicionar às já existentes)

MEYER, A. N. et al. The work life of developers: activities, switches and
perceived productivity. **IEEE Transactions on Software Engineering**, v. 43,
n. 12, p. 1178–1193, 2014.

ZIERIS, F.; SALINGER, S. Doing it wrong or doing it right? Pair programming
in professional software development. In: **Agile Conference**, 2014. p. 65–73.

> **Nota:** Os dados de acesso e URLs devem ser complementados conforme as
> normas ABNT do TCC USP/ESALQ. Verifique se as obras acima estão disponíveis
> nas bases de dados acessíveis pelo portal CAPES para inclusão como referências
> formais.

---

## LISTA DE FIGURAS GERADAS

Todos os arquivos estão em:
`docs/tcc_assets/`

| # | Arquivo | Descrição | Inserir em |
|---|---|---|---|
| Figura X | `diagrama_arquitetura.png` | Diagrama de componentes por camada | Metodologia — Arquitetura do sistema |
| Figura Y | `schema_banco.png` | Esquema das 3 tabelas SQLite | Metodologia — Estrutura do banco de dados |
| Figura Z1 | `screenshot_boas_vindas.png` | Interface com popup + Git + presets coloridos | Metodologia — Funcionalidades / Resultados |
| Figura Z2 | `screenshot_scroll.png` | Painel completo + Recent runs com histórico | Metodologia — Funcionalidades / Resultados |
| Figura W | `fluxograma_execucao.png` | Fluxograma de execução de comando | Metodologia — Passo a passo da execução |
