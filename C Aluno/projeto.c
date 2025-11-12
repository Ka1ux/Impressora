#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= Config DLL ======================= */
static HMODULE g_hDll = NULL;

/* Conven��o de chamada (Windows): __stdcall */
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
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

/* ======================= Ponteiros ======================= */
static AbreConexaoImpressora_t        AbreConexaoImpressora        = NULL;
static FechaConexaoImpressora_t       FechaConexaoImpressora       = NULL;
static ImpressaoTexto_t               ImpressaoTexto               = NULL;
static Corte_t                        Corte                        = NULL;
static ImpressaoQRCode_t              ImpressaoQRCode              = NULL;
static ImpressaoCodigoBarras_t        ImpressaoCodigoBarras        = NULL;
static AvancaPapel_t                  AvancaPapel                  = NULL;
static AbreGavetaElgin_t              AbreGavetaElgin              = NULL;
static AbreGaveta_t                   AbreGaveta                   = NULL;
static SinalSonoro_t                  SinalSonoro                  = NULL;
static ImprimeXMLSAT_t                ImprimeXMLSAT                = NULL;
static ImprimeXMLCancelamentoSAT_t    ImprimeXMLCancelamentoSAT    = NULL;
static InicializaImpressora_t         InicializaImpressora         = NULL;

/* ======================= Configura��o ======================= */
static int   g_tipo      = 1;
static char  g_modelo[64] = "i9";
static char  g_conexao[128] = "USB";
static int   g_parametro = 0;
static int   g_conectada = 0;

/* ======================= Utilidades ======================= */
#define LOAD_FN(h, name)                                                         \
    do {                                                                         \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name);                    \
        if (!(name)) {                                                           \
            fprintf(stderr, "Falha ao resolver s�mbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ======================= Fun��es para manipular a DLL ======================= */
static int carregarFuncoes(void)
{
    g_hDll = LoadLibraryA("E1_Impressora01.dll");
    if (!g_hDll) {
        DWORD erro = GetLastError();
        fprintf(stderr, "\n========================================\n");
        fprintf(stderr, "AVISO: DLL nao encontrada!\n");
        fprintf(stderr, "Erro ao carregar E1_Impressora01.dll (erro=%lu)\n", erro);
        fprintf(stderr, "\nIsso e normal se:\n");
        fprintf(stderr, "- A impressora Elgin nao esta instalada\n");
        fprintf(stderr, "- A DLL nao esta no mesmo diretorio do programa\n");
        fprintf(stderr, "- Voce esta apenas testando o codigo\n");
        fprintf(stderr, "========================================\n\n");
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

static void liberarBiblioteca(void)
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

/* ======================= Fun��es a serem implementadas pelos alunos ======================= */

static void exibirMenu(void)
{
    printf("\n========== MENU PRINCIPAL ==========\n");
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
    printf("====================================\n");
    printf("Escolha uma opcao: ");
}

static void configurarConexao(void)
{
    printf("\n=== Configuracao de Conexao ===\n");
    
    printf("Tipo (1=USB, 2=Serial, etc): ");
    scanf("%d", &g_tipo);
    flush_entrada();
    
    printf("Modelo (ex: i9): ");
    if (fgets(g_modelo, sizeof(g_modelo), stdin) != NULL) {
        size_t len = strlen(g_modelo);
        if (len > 0 && g_modelo[len-1] == '\n') {
            g_modelo[len-1] = '\0';
        }
    }
    
    printf("Conexao (ex: USB, COM1): ");
    if (fgets(g_conexao, sizeof(g_conexao), stdin) != NULL) {
        size_t len = strlen(g_conexao);
        if (len > 0 && g_conexao[len-1] == '\n') {
            g_conexao[len-1] = '\0';
        }
    }
    
    printf("Parametro: ");
    scanf("%d", &g_parametro);
    flush_entrada();
    
    printf("Configuracao salva!\n");
}

static void abrirConexao(void)
{
    if (g_conectada) {
        printf("Conexao ja esta aberta!\n");
        return;
    }
    
    if (!AbreConexaoImpressora) {
        printf("Erro: Funcao AbreConexaoImpressora nao carregada!\n");
        return;
    }
    
    int ret = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);
    if (ret == 0) {
        g_conectada = 1;
        printf("Conexao aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir conexao (codigo: %d)\n", ret);
    }
}

static void fecharConexao(void)
{
    if (!g_conectada) {
        printf("Nenhuma conexao aberta!\n");
        return;
    }
    
    if (!FechaConexaoImpressora) {
        printf("Erro: Funcao FechaConexaoImpressora nao carregada!\n");
        return;
    }
    
    int ret = FechaConexaoImpressora();
    if (ret == 0) {
        g_conectada = 0;
        printf("Conexao fechada com sucesso!\n");
    } else {
        printf("Erro ao fechar conexao (codigo: %d)\n", ret);
    }
}

static void imprimirTexto(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    char texto[512];
    printf("\nDigite o texto a ser impresso: ");
    if (fgets(texto, sizeof(texto), stdin) != NULL) {
        size_t len = strlen(texto);
        if (len > 0 && texto[len-1] == '\n') {
            texto[len-1] = '\0';
        }
    }
    
    if (!ImpressaoTexto || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        return;
    }
    
    int ret = ImpressaoTexto(texto, 0, 0, 0);
    if (ret == 0) {
        AvancaPapel(10);
        Corte(0);
        printf("Texto impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir texto (codigo: %d)\n", ret);
    }
}

static void imprimirQRCode(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    char qrtexto[256];
    printf("\nDigite o conteudo do QR Code: ");
    if (fgets(qrtexto, sizeof(qrtexto), stdin) != NULL) {
        size_t len = strlen(qrtexto);
        if (len > 0 && qrtexto[len-1] == '\n') {
            qrtexto[len-1] = '\0';
        }
    }
    
    if (!ImpressaoQRCode || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        return;
    }
    
    int ret = ImpressaoQRCode(qrtexto, 6, 4);
    if (ret == 0) {
        AvancaPapel(10);
        Corte(0);
        printf("QR Code impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir QR Code (codigo: %d)\n", ret);
    }
}

static void imprimirCodigoBarras(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    if (!ImpressaoCodigoBarras || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        return;
    }
    
    int ret = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    if (ret == 0) {
        AvancaPapel(10);
        Corte(0);
        printf("Codigo de barras impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir codigo de barras (codigo: %d)\n", ret);
    }
}

static void imprimirXMLSAT(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    FILE *arquivo = fopen("./XMLSAT.xml", "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir o arquivo XMLSAT.xml\n");
        return;
    }
    
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    
    char *conteudo = (char*)malloc(tamanho + 1);
    if (!conteudo) {
        printf("Erro: Falha ao alocar memoria\n");
        fclose(arquivo);
        return;
    }
    
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    if (!ImprimeXMLSAT || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        free(conteudo);
        return;
    }
    
    int ret = ImprimeXMLSAT(conteudo, 0);
    free(conteudo);
    
    if (ret == 0) {
        AvancaPapel(10);
        Corte(0);
        printf("XML SAT impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir XML SAT (codigo: %d)\n", ret);
    }
}

static void imprimirXMLCancelamentoSAT(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    FILE *arquivo = fopen("./CANC_SAT.xml", "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir o arquivo CANC_SAT.xml\n");
        return;
    }
    
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    
    char *conteudo = (char*)malloc(tamanho + 1);
    if (!conteudo) {
        printf("Erro: Falha ao alocar memoria\n");
        fclose(arquivo);
        return;
    }
    
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    const char *assinatura = 
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
    
    if (!ImprimeXMLCancelamentoSAT || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        free(conteudo);
        return;
    }
    
    int ret = ImprimeXMLCancelamentoSAT(conteudo, assinatura, 0);
    free(conteudo);
    
    if (ret == 0) {
        AvancaPapel(10);
        Corte(0);
        printf("XML Cancelamento SAT impresso com sucesso!\n");
    } else {
        printf("Erro ao imprimir XML Cancelamento SAT (codigo: %d)\n", ret);
    }
}

static void abrirGavetaElginOpc(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    if (!AbreGavetaElgin) {
        printf("Erro: Funcao AbreGavetaElgin nao carregada!\n");
        return;
    }
    
    int ret = AbreGavetaElgin(1, 50, 50);
    if (ret == 0) {
        printf("Gaveta Elgin aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir gaveta Elgin (codigo: %d)\n", ret);
    }
}

static void abrirGavetaOpc(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    if (!AbreGaveta) {
        printf("Erro: Funcao AbreGaveta nao carregada!\n");
        return;
    }
    
    int ret = AbreGaveta(1, 5, 10);
    if (ret == 0) {
        printf("Gaveta aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir gaveta (codigo: %d)\n", ret);
    }
}

static void emitirSinalSonoro(void)
{
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    if (!SinalSonoro) {
        printf("Erro: Funcao SinalSonoro nao carregada!\n");
        return;
    }
    
    int ret = SinalSonoro(4, 50, 5);
    if (ret == 0) {
        printf("Sinal sonoro emitido com sucesso!\n");
    } else {
        printf("Erro ao emitir sinal sonoro (codigo: %d)\n", ret);
    }
}

/* ======================= Funo principal ======================= */
int main(void)
{
    if (!carregarFuncoes()) {
        return 1;
    }

    int opcao = 0;
    while (1) {
        exibirMenu();
        scanf("%d", &opcao);
        flush_entrada();
        
        switch (opcao) {
            case 1:
                configurarConexao();
                break;
            case 2:
                abrirConexao();
                break;
            case 3:
                fecharConexao();
                break;
            case 4:
                imprimirTexto();
                break;
            case 5:
                imprimirQRCode();
                break;
            case 6:
                imprimirCodigoBarras();
                break;
            case 7:
                imprimirXMLSAT();
                break;
            case 8:
                imprimirXMLCancelamentoSAT();
                break;
            case 9:
                abrirGavetaElginOpc();
                break;
            case 10:
                abrirGavetaOpc();
                break;
            case 11:
                emitirSinalSonoro();
                break;
            case 12:
                if (InicializaImpressora) {
                    int ret = InicializaImpressora();
                    if (ret == 0) {
                        printf("Impressora inicializada com sucesso!\n");
                    } else {
                        printf("Erro ao inicializar impressora (codigo: %d)\n", ret);
                    }
                } else {
                    printf("Erro: Funcao InicializaImpressora nao carregada!\n");
                }
                break;
            case 0:
                if (g_conectada) {
                    fecharConexao();
                }
                liberarBiblioteca();
                printf("Encerrando programa...\n");
                return 0;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    }
}
 