// ===== PARTE 1 - Contexto e infraestrutura (Pessoa 1) =====
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// Handle global para a DLL da impressora Elgin
// Armazena a referência da biblioteca carregada dinamicamente
static HMODULE g_hDll = NULL;

// Define a convenção de chamada para funções da DLL
// WINAPI garante compatibilidade com funções exportadas da DLL
#ifndef CALLCONV
#define CALLCONV WINAPI
#endif

// Definição dos tipos de função da DLL E1_Impressora01.dll
// Cada typedef define a assinatura de uma função exportada pela DLL
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
typedef int (CALLCONV *Corte_t)(int);
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
typedef int (CALLCONV *AvancaPapel_t)(int);
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
typedef int (CALLCONV *InicializaImpressora_t)(void);

// Ponteiros para funções da DLL
// Inicializados como NULL e preenchidos após carregar a DLL
// Permitem chamar as funções da DLL de forma dinâmica
static AbreConexaoImpressora_t AbreConexaoImpressora = NULL;
static FechaConexaoImpressora_t FechaConexaoImpressora = NULL;
static ImpressaoTexto_t ImpressaoTexto = NULL;
static Corte_t Corte = NULL;
static ImpressaoQRCode_t ImpressaoQRCode = NULL;
static ImpressaoCodigoBarras_t ImpressaoCodigoBarras = NULL;
static AvancaPapel_t AvancaPapel = NULL;
static AbreGavetaElgin_t AbreGavetaElgin = NULL;
static AbreGaveta_t AbreGaveta = NULL;
static SinalSonoro_t SinalSonoro = NULL;
static ImprimeXMLSAT_t ImprimeXMLSAT = NULL;
static ImprimeXMLCancelamentoSAT_t ImprimeXMLCancelamentoSAT = NULL;
static InicializaImpressora_t InicializaImpressora = NULL;

// Variáveis globais de configuração da impressora
static int g_tipo = 1;                    // Tipo de conexão: 1=USB, 2=Serial
static char g_modelo[64] = "i9";          // Modelo da impressora (ex: i9, i7, etc)
static char g_conexao[128] = "USB";       // String de conexão (USB, COM1, etc)
static int g_parametro = 0;               // Parâmetro adicional para conexão
static int g_conectada = 0;               // Flag: 1=conectada, 0=desconectada

// ===== PARTE 2 - Carregamento dinâmico e utilidades (Pessoa 2) =====

// Macro para carregar uma função da DLL dinamicamente
// h: handle da DLL carregada
// name: nome da função a ser carregada
// Usa GetProcAddress para obter o endereço da função exportada
#define LOAD_FN(h, name) \
    do { \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name); \
        if (!(name)) { \
            fprintf(stderr, "Erro ao carregar %s\n", #name); \
            return 0; \
        } \
    } while (0)

// Funções auxiliares para manipulação de entrada do usuário

// Limpa o buffer de entrada após scanf
// Remove caracteres restantes (incluindo '\n') até encontrar quebra de linha ou EOF
static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Remove o caractere de quebra de linha do final de uma string
// Útil após usar fgets() que inclui '\n' na string lida
static void remover_quebra_linha(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len-1] == '\n') str[len-1] = '\0';
}

// Verifica se a impressora está conectada antes de executar operações
// Retorna: 1 se conectada, 0 se não conectada
// Exibe mensagem de erro se não estiver conectada
static int verificar_conexao(void) {
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return 0;
    }
    return 1;
}

// Finaliza a impressão avançando o papel e cortando
// Avança 1 linha de papel (economiza papel comparado a valores maiores)
// Realiza corte parcial (0) após avançar o papel
// Chamada automaticamente após cada operação de impressão
static void finalizar_impressao(void) {
    if (AvancaPapel && Corte) {
        AvancaPapel(3);  // Avança 1 linha (economia de papel)
        Corte(0);        // Corte parcial
    }
}

// Carrega a DLL da impressora e todas as funções necessárias
// Retorna: 1 se sucesso, 0 se falha
// Carrega dinamicamente a biblioteca E1_Impressora01.dll e resolve os endereços
// de todas as funções exportadas necessárias para o funcionamento do sistema
static int carregarFuncoes(void) {
    g_hDll = LoadLibraryA("E1_Impressora01.dll");
    if (!g_hDll) {
        printf("AVISO: DLL nao encontrada! (Isso e normal se nao tiver a impressora)\n");
        return 0;
    }

    LOAD_FN(g_hDll, AbreConexaoImpressora);
    LOAD_FN(g_hDll, FechaConexaoImpressora);
    LOAD_FN(g_hDll, ImpressaoTexto);
    LOAD_FN(g_hDll, Corte);
    LOAD_FN(g_hDll, ImpressaoQRCode);
    LOAD_FN(g_hDll, ImpressaoCodigoBarras);
    LOAD_FN(g_hDll, AvancaPapel);
    LOAD_FN(g_hDll, AbreGavetaElgin);
    LOAD_FN(g_hDll, AbreGaveta);
    LOAD_FN(g_hDll, SinalSonoro);
    LOAD_FN(g_hDll, ImprimeXMLSAT);
    LOAD_FN(g_hDll, ImprimeXMLCancelamentoSAT);
    LOAD_FN(g_hDll, InicializaImpressora);
    return 1;
}

// Libera a DLL da memória antes de encerrar o programa
// Descarrega a biblioteca usando FreeLibrary e reseta o handle
// Importante para evitar vazamentos de memória
static void liberarBiblioteca(void) {
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

// ===== PARTE 3 - Menu e gerenciamento de conexão (Pessoa 3) =====

// Exibe o menu principal do sistema com todas as opções disponíveis
// Menu interativo que permite ao usuário escolher as operações a realizar
static void exibirMenu(void) {
    printf("\n========== MENU ==========\n");
    printf("1. Configurar Conexao\n");
    printf("2. Abrir Conexao\n");
    printf("3. Imprimir Texto\n");
    printf("4. Imprimir QR Code\n");
    printf("5. Imprimir Codigo de Barras\n");
    printf("6. Imprimir XML SAT\n");
    printf("7. Imprimir XML Cancelamento SAT\n");
    printf("8. Abrir Gaveta Elgin\n");
    printf("9. Abrir Gaveta\n");
    printf("10. Emitir Sinal Sonoro\n");
    printf("0. Sair\n");
    printf("========================\n");
    printf("Opcao: ");
}

// Configura os parâmetros de conexão da impressora
// Solicita ao usuário: tipo de conexão, modelo, string de conexão e parâmetro adicional
// As configurações são armazenadas nas variáveis globais para uso posterior
static void configurarConexao(void) {
    printf("\n=== Configuracao ===\n");
    printf("Tipo (1=USB, 2=Serial): ");
    scanf("%d", &g_tipo);
    flush_entrada();
    
    printf("Modelo: ");
    if (fgets(g_modelo, sizeof(g_modelo), stdin)) remover_quebra_linha(g_modelo);
    
    printf("Conexao: ");
    if (fgets(g_conexao, sizeof(g_conexao), stdin)) remover_quebra_linha(g_conexao);
    
    printf("Parametro: ");
    scanf("%d", &g_parametro);
    flush_entrada();
    
    printf("Configuracao salva!\n");
}

// Abre a conexão com a impressora usando as configurações definidas
// Verifica se já está conectada antes de tentar abrir novamente
// Retorno 0 indica sucesso, outros valores indicam erro
// Atualiza a flag g_conectada para indicar o estado da conexão
static void abrirConexao(void) {
    if (g_conectada) {
        printf("Conexao ja esta aberta!\n");
        return;
    }
    
    int ret = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);
    if (ret == 0) {
        g_conectada = 1;
        printf("Conexao aberta!\n");
    } else {
        printf("Erro ao abrir (codigo: %d)\n", ret);
    }
}

// Fecha a conexão com a impressora
// Verifica se há conexão aberta antes de tentar fechar
// Retorno 0 indica sucesso, outros valores indicam erro
// Atualiza a flag g_conectada para 0 após fechar com sucesso
static void fecharConexao(void) {
    if (!g_conectada) {
        printf("Nenhuma conexao aberta!\n");
        return;
    }
    
    int ret = FechaConexaoImpressora();
    if (ret == 0) {
        g_conectada = 0;
        printf("Conexao fechada!\n");
    } else {
        printf("Erro ao fechar (codigo: %d)\n", ret);
    }
}

// ===== PARTE 4 - Rotinas de impressão (Pessoa 4) =====

// Imprime um texto simples na impressora
// Solicita o texto ao usuário (até 512 caracteres)
// Parâmetros da impressão: texto, alinhamento(0), estilo(0), tamanho(0)
// Após imprimir, finaliza com avanço de papel e corte
static void imprimirTexto(void) {
    if (!verificar_conexao()) return;
    
    char texto[512];
    printf("\nTexto: ");
    if (fgets(texto, sizeof(texto), stdin)) {
        remover_quebra_linha(texto);
        int ret = ImpressaoTexto(texto, 0, 0, 0);
        if (ret == 0) {
            finalizar_impressao();
            printf("Texto impresso!\n");
        } else {
            printf("Erro (codigo: %d)\n", ret);
        }
    }
}

// Imprime um QR Code na impressora
// Solicita o conteúdo do QR Code ao usuário (até 256 caracteres)
// Parâmetros: texto do QR Code, tamanho(6), correção de erro(4)
// Após imprimir, finaliza com avanço de papel e corte
static void imprimirQRCode(void) {
    if (!verificar_conexao()) return;
    
    char qrtexto[256];
    printf("\nQR Code: ");
    if (fgets(qrtexto, sizeof(qrtexto), stdin)) {
        remover_quebra_linha(qrtexto);
        int ret = ImpressaoQRCode(qrtexto, 6, 4);
        if (ret == 0) {
            finalizar_impressao();
            printf("QR Code impresso!\n");
        } else {
            printf("Erro (codigo: %d)\n", ret);
        }
    }
}

// Imprime um código de barras na impressora
// Usa código de exemplo fixo: tipo(8), código("{A012345678912"), altura(100), largura(2), posição(3)
// Parâmetros: tipo de código, string do código, altura em pontos, largura, posição do texto
// Após imprimir, finaliza com avanço de papel e corte
static void imprimirCodigoBarras(void) {
    if (!verificar_conexao()) return;
    
    int ret = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    if (ret == 0) {
        finalizar_impressao();
        printf("Codigo de barras impresso!\n");
    } else {
        printf("Erro (codigo: %d)\n", ret);
    }
}

// Lê o conteúdo completo de um arquivo XML para memória
// Parâmetro: nome do arquivo a ser lido
// Retorna: ponteiro para string alocada dinamicamente com o conteúdo, ou NULL se erro
// O buffer retornado deve ser liberado com free() após o uso
static char* ler_arquivo(const char *nome) {
    FILE *f = fopen(nome, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *buf = (char*)malloc(tam + 1);
    if (buf) {
        fread(buf, 1, tam, f);
        buf[tam] = '\0';
    }
    fclose(f);
    return buf;
}

// Imprime um cupom fiscal SAT (Sistema Autenticador e Transmissor) a partir de arquivo XML
// Lê o arquivo XMLSAT.xml do diretório atual
// O XML contém os dados do cupom fiscal que será impresso
// Após imprimir, finaliza com avanço de papel e corte
// Libera a memória alocada para o conteúdo do arquivo
static void imprimirXMLSAT(void) {
    if (!verificar_conexao()) return;
    
    char *conteudo = ler_arquivo("./XMLSAT.xml");
    if (!conteudo) {
        printf("Erro ao abrir XMLSAT.xml\n");
        return;
    }
    
    int ret = ImprimeXMLSAT(conteudo, 0);
    free(conteudo);
    
    if (ret == 0) {
        finalizar_impressao();
        printf("XML SAT impresso!\n");
    } else {
        printf("Erro (codigo: %d)\n", ret);
    }
}

// Imprime um cupom de cancelamento SAT a partir de arquivo XML
// Lê o arquivo CANC_SAT.xml do diretório atual
// Requer uma assinatura digital para autenticar o cancelamento
// A assinatura é fornecida como string codificada em Base64
// Após imprimir, finaliza com avanço de papel e corte
// Libera a memória alocada para o conteúdo do arquivo
static void imprimirXMLCancelamentoSAT(void) {
    if (!verificar_conexao()) return;
    
    char *conteudo = ler_arquivo("./CANC_SAT.xml");
    if (!conteudo) {
        printf("Erro ao abrir CANC_SAT.xml\n");
        return;
    }
    
    const char *assinatura = "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";
    
    int ret = ImprimeXMLCancelamentoSAT(conteudo, assinatura, 0);
    free(conteudo);
    
    if (ret == 0) {
        finalizar_impressao();
        printf("XML Cancelamento impresso!\n");
    } else {
        printf("Erro (codigo: %d)\n", ret);
    }
}

// ===== PARTE 5 - Acessórios, reset e loop principal (Pessoa 5) =====

// Abre a gaveta de dinheiro da impressora Elgin
// Parâmetros: índice da gaveta(1), tempo de abertura em ms(50), tempo de fechamento em ms(50)
// Usado para abrir a gaveta de dinheiro acoplada à impressora
static void abrirGavetaElginOpc(void) {
    if (!verificar_conexao()) return;
    
    int ret = AbreGavetaElgin(1, 50, 50);
    printf(ret == 0 ? "Gaveta aberta!\n" : "Erro (codigo: %d)\n", ret);
}

// Abre a gaveta de dinheiro genérica (compatível com vários modelos)
// Parâmetros: índice da gaveta(1), tempo de abertura em ms(5), tempo de fechamento em ms(10)
// Função alternativa para modelos que não suportam AbreGavetaElgin
static void abrirGavetaOpc(void) {
    if (!verificar_conexao()) return;
    
    int ret = AbreGaveta(1, 5, 10);
    printf(ret == 0 ? "Gaveta aberta!\n" : "Erro (codigo: %d)\n", ret);
}

// Emite um sinal sonoro (bip) na impressora
// Parâmetros: quantidade de bips(4), intervalo entre bips em ms(50), duração de cada bip em ms(5)
// Útil para alertas ou confirmações sonoras
static void emitirSinalSonoro(void) {
    if (!verificar_conexao()) return;
    
    int ret = SinalSonoro(4, 50, 5);
    printf(ret == 0 ? "Sinal emitido!\n" : "Erro (codigo: %d)\n", ret);
}

// Reseta o buffer interno da impressora
// Utiliza a função InicializaImpressora da DLL para limpar o spool/buffer
// Só pode ser executado com a impressora conectada
// Útil quando o equipamento fica travado ou acumulando comandos
static void resetarBuffer(void) {
    if (!verificar_conexao()) return;

    if (!InicializaImpressora) {
        printf("Funcao de reset nao disponivel!\n");
        return;
    }

    int ret = InicializaImpressora();
    if (ret == 0) {
        printf("Buffer da impressora resetado!\n");
    } else {
        printf("Erro ao resetar buffer (codigo: %d)\n", ret);
    }
}

// Função principal do programa
// Inicializa o sistema carregando a DLL da impressora
// Executa um loop infinito exibindo o menu e processando as opções do usuário
// Case 2 abre conexão; case 0 reseta buffer, fecha conexão (se aberta) e encerra
// Retorna 1 se falhar ao carregar a DLL, 0 ao encerrar normalmente
int main(void) {
    if (!carregarFuncoes()) return 1;

    int opcao;
    while (1) {
        exibirMenu();
        scanf("%d", &opcao);
        flush_entrada();
        
        switch (opcao) {
            case 1: configurarConexao(); break;
            case 2: abrirConexao(); break;
            case 3: imprimirTexto(); break;
            case 4: imprimirQRCode(); break;
            case 5: imprimirCodigoBarras(); break;
            case 6: imprimirXMLSAT(); break;
            case 7: imprimirXMLCancelamentoSAT(); break;
            case 8: abrirGavetaElginOpc(); break;
            case 9: abrirGavetaOpc(); break;
            case 10: emitirSinalSonoro(); break;
            case 0:
                // Opção 0: Resetar buffer, fechar conexão e sair
                if (g_conectada) {
                    resetarBuffer();
                    fecharConexao();
                }
                liberarBiblioteca();
                return 0;
            default: printf("Opcao invalida!\n"); break;
        }
    }
}


