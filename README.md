# Impressora

## 📘 Descrição
O projeto **Impressora** tem como objetivo criar uma aplicação para **gerenciar e enviar trabalhos de impressão** diretamente a impressoras conectadas ao sistema, simulando ou controlando dispositivos de saída reais.  
O projeto foi criado para fins de **aprendizado, automação e integração com hardware** dentro do contexto de estudos de Engenharia da Computação.

---

## ⚙️ Funcionalidades Principais
- 🖨️ Envio de texto e documentos para impressão.  
- 📂 Leitura e processamento de arquivos (TXT, PDF, IMG, etc).  
- 🧩 Seleção de impressora instalada no sistema.  
- 🧾 Configuração de layout, margens e formato de página.  
- 💾 Registro de logs de impressões realizadas.  
- 🔌 Comunicação direta com portas USB, LPT ou rede.  

---

## 🧠 Motivação
O projeto nasceu da curiosidade em **entender como softwares se comunicam com impressoras** e como sistemas operacionais gerenciam filas de impressão.  
Foi uma oportunidade de estudar:
- APIs e drivers de impressão.  
- Comunicação com hardware em baixo nível.  
- Gerenciamento de arquivos e buffers.  
- Estrutura modular de software.  

---

## 🛠️ Tecnologias Utilizadas
| Categoria | Ferramenta / Tecnologia |
|------------|--------------------------|
| Linguagem | C |
| Sistema | Windows |
| Compilador | GCC (MinGW) / MSVC |
| Bibliotecas | `windows.h`, `winspool.drv` |
| Controle de versão | Git + GitHub |
| Documentação | Markdown (`README.md`) |

---

## 📂 Estrutura do Projeto
```
Impressora/
├── src/                    # Código-fonte principal
│   ├── main.c              # Ponto de entrada
│   ├── printer.c           # Funções de comunicação com impressora
│   ├── layout.c            # Gerenciamento de layout e formatação
│   └── utils.c             # Funções auxiliares
│
├── include/                # Arquivos de cabeçalho (.h)
│   ├── printer.h
│   ├── layout.h
│   └── utils.h
│
├── docs/                   # Documentação técnica
│   └── especificacao.md
│
├── tests/                  # Testes unitários
│   └── test_printer.c
│
├── Makefile                # Script de compilação
└── README.md               # Este arquivo
```

---

## 🚀 Instalação e Execução

### 🔧 Pré-requisitos
- Sistema: **Windows 10 ou superior**
- Compilador: **MinGW ou Visual Studio**
- Impressora instalada e configurada
- Git instalado

### 📦 Instalação
```bash
git clone https://github.com/Ka1ux/Impressora.git
cd Impressora
make
```

### ▶️ Execução
Após a compilação bem-sucedida:
```bash
./Impressora.exe
```

Caso use Visual Studio:
1. Abra `Impressora.sln`
2. Compile no modo **Release**
3. Clique em **Executar**

---

## ⚡ Exemplo de Uso
```bash
# Executar o programa e imprimir um arquivo de texto
./Impressora.exe arquivo.txt

# Exemplo de saída esperada
Conectando à impressora padrão...
Enviando arquivo: arquivo.txt
Impressão concluída com sucesso!
```

---

## 🧩 Configuração
Você pode personalizar parâmetros de impressão no arquivo `config.ini` (se existir).  
Exemplo:
```ini
[printer]
name = HP_LaserJet_1020
orientation = portrait
margins = 5
copies = 2
```

---

## 🧪 Testes
Para rodar testes unitários:
```bash
make test
```
Ou compile manualmente:
```bash
gcc tests/test_printer.c -o test_printer.exe
./test_printer.exe
```

---
## Pessoas
- Kaua Barroso
- Erick Maycon
- Rafael Fróes
- 00000000
- Jhonata
---

## 📝 Licença
Este projeto é distribuído sob a licença **MIT License** — veja o arquivo `LICENSE` para mais detalhes.

---

## 🧭 Roadmap (Próximos Passos)
- [ ] Adicionar suporte a PDF direto  
- [ ] Interface gráfica simples (GUI)  
- [ ] Logs mais detalhados com data/hora  
- [ ] Suporte a múltiplas impressoras  
- [ ] Modo de simulação (sem impressão real)
      
---

## 📚 Referências
- Documentação Microsoft WinAPI (Impressão): https://learn.microsoft.com/en-us/windows/win32/printdocs/
- StackOverflow – tópicos sobre impressão em C  
- Tutoriais sobre controle de impressoras com `winspool.drv`
