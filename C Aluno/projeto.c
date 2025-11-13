#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static HMODULE g_hDll = NULL;

#ifndef CALLCONV
#define CALLCONV WINAPI
#endif

// Tipos de função da DLL
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

// Ponteiros para funções
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

// Configurações
static int g_tipo = 1;
static char g_modelo[64] = "i9";
static char g_conexao[128] = "USB";
static int g_parametro = 0;
static int g_conectada = 0;

// Macro para carregar função
#define LOAD_FN(h, name) \
    do { \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name); \
        if (!(name)) { \
            fprintf(stderr, "Erro ao carregar %s\n", #name); \
            return 0; \
        } \
    } while (0)

// Funções auxiliares
static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void remover_quebra_linha(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len-1] == '\n') str[len-1] = '\0';
}

// Verifica se está conectado
static int verificar_conexao(void) {
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return 0;
    }
    return 1;
}

// Finaliza impressão (avança papel e corta)
static void finalizar_impressao(void) {
    if (AvancaPapel && Corte) {
        AvancaPapel(10);
        Corte(0);
    }
}

// Carrega DLL
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

static void liberarBiblioteca(void) {
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

// Menu
static void exibirMenu(void) {
    printf("\n========== MENU ==========\n");
    printf("1. Configurar Conexao\n");
    printf("2. Abrir Conexao\n");
    printf("3. Fechar Conexao\n");
    printf("4. Imprimir Texto\n");
    printf("5. Imprimir QR Code\n");
    printf("6. Imprimir Codigo de Barras\n");
    printf("7. Imprimir XML SAT\n");
    printf("8. Imprimir XML Cancelamento SAT\n");
    printf("9. Abrir Gaveta Elgin\n");
    printf("10. Abrir Gaveta\n");
    printf("11. Emitir Sinal Sonoro\n");
    printf("12. Inicializar Impressora\n");
    printf("0. Sair\n");
    printf("========================\n");
    printf("Opcao: ");
}

// Configura conexão
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

// Abre conexão
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

// Fecha conexão
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

// Imprime texto
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

// Imprime QR Code
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

// Imprime código de barras
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

// Lê arquivo XML
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

// Imprime XML SAT
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

// Imprime XML Cancelamento SAT
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

// Abre gaveta Elgin
static void abrirGavetaElginOpc(void) {
    if (!verificar_conexao()) return;
    
    int ret = AbreGavetaElgin(1, 50, 50);
    printf(ret == 0 ? "Gaveta aberta!\n" : "Erro (codigo: %d)\n", ret);
}

// Abre gaveta genérica
static void abrirGavetaOpc(void) {
    if (!verificar_conexao()) return;
    
    int ret = AbreGaveta(1, 5, 10);
    printf(ret == 0 ? "Gaveta aberta!\n" : "Erro (codigo: %d)\n", ret);
}

// Emite sinal sonoro
static void emitirSinalSonoro(void) {
    if (!verificar_conexao()) return;
    
    int ret = SinalSonoro(4, 50, 5);
    printf(ret == 0 ? "Sinal emitido!\n" : "Erro (codigo: %d)\n", ret);
}

// Main
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
            case 3: fecharConexao(); break;
            case 4: imprimirTexto(); break;
            case 5: imprimirQRCode(); break;
            case 6: imprimirCodigoBarras(); break;
            case 7: imprimirXMLSAT(); break;
            case 8: imprimirXMLCancelamentoSAT(); break;
            case 9: abrirGavetaElginOpc(); break;
            case 10: abrirGavetaOpc(); break;
            case 11: emitirSinalSonoro(); break;
            case 12:
                if (InicializaImpressora) {
                    int ret = InicializaImpressora();
                    printf(ret == 0 ? "Impressora inicializada!\n" : "Erro (codigo: %d)\n", ret);
                }
                break;
            case 0:
                if (g_conectada) fecharConexao();
                liberarBiblioteca();
                return 0;
            default: printf("Opcao invalida!\n"); break;
        }
    }
}

