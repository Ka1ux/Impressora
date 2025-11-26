# Impressora

## 📘 Descrição
O projeto **Impressora** tem como objetivo criar uma aplicação para **gerenciar e enviar trabalhos de impressão** diretamente a impressoras conectadas ao sistema, simulando ou controlando dispositivos de saída reais.  
O projeto foi criado para fins de **aprendizado, automação e integração com hardware** dentro do contexto de estudos de Engenharia da Computação.

---

## ⚙️ Funcionalidades Principais

🖨️ **Impressão de texto**  
Envio de texto simples diretamente para a impressora Elgin Bematech i9 usando comandos ESC/POS.

🔢 **Impressão de código de barras**  
Suporte para impressão de código de barras nos padrões aceitos pela impressora i9 (via comandos ESC/POS).

🔳 **Impressão de QR Code**  
Geração e envio de QR Code diretamente para a impressora, utilizando os comandos específicos da i9.

🔌 **Comunicação direta com a impressora**  
Envio de dados RAW diretamente pela porta configurada no Windows (USB, COM ou porta padrão da i9).

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

A impressora Elgin Bematech i9 trabalha com comandos ESC/POS.  
Todas as configurações do projeto são feitas **dentro do código**, e não através de arquivos externos.

Aqui estão as únicas configurações necessárias:

---

### 🔌Porta da Impressora
```
Você precisa indicar o nome da impressora instalada no Windows (como aparece no Painel de Controle).
```
---
## Pessoas
- Kaua Barroso
- Erick Maycon
- Rafael Fróes
- Matheus Derllova
- Jhonata Lion
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
