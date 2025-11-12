/* ======================= BIBLIOTECAS ======================= */
/* stdio.h: Funções de entrada/saída (printf, scanf, fgets, etc) */
/* stdlib.h: Funções utilitárias (malloc, free, etc) */
/* string.h: Funções de manipulação de strings (strlen, etc) */
/* windows.h: Funções específicas do Windows (LoadLibrary, GetProcAddress, etc) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= CONFIGURAÇÃO DA DLL ======================= */
/* Handle (ponteiro) para a biblioteca dinâmica carregada em memória */
/* NULL indica que a DLL ainda não foi carregada */
static HMODULE g_hDll = NULL;

/* Conven��o de chamada (Windows): __stdcall */
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
/* Abre conexão com a impressora: tipo, modelo, conexão, parâmetro */
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
/* Fecha a conexão com a impressora */
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
/* Imprime texto: texto, alinhamento, fonte, tamanho */
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
/* Realiza corte do papel: tipo de corte */
typedef int (CALLCONV *Corte_t)(int);
/* Imprime QR Code: conteúdo, tamanho, correção de erro */
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
/* Imprime código de barras: tipo, dados, altura, largura, posição */
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
/* Avança papel: quantidade de linhas */
typedef int (CALLCONV *AvancaPapel_t)(int);
/* Abre gaveta Elgin: pino, tempo1, tempo2 */
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
/* Abre gaveta genérica: pino, tempo1, tempo2 */
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
/* Emite sinal sonoro: quantidade, tempo on, tempo off */
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
/* Imprime XML do SAT: conteúdo XML, tipo */
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
/* Imprime XML de cancelamento SAT: XML, assinatura, tipo */
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
/* Inicializa a impressora */
typedef int (CALLCONV *InicializaImpressora_t)(void);

/* ======================= PONTEIROS PARA FUNÇÕES DA DLL ======================= */
/* Armazenam os endereços das funções carregadas dinamicamente da DLL */
/* Inicializados como NULL e preenchidos quando a DLL é carregada */
/* Se NULL, significa que a função não foi encontrada ou a DLL não foi carregada */
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

/**
 * Libera a DLL da memória quando o programa termina
 * 
 * FreeLibrary decrementa o contador de referências da DLL
 * A DLL só é realmente descarregada quando o contador chega a zero
 */
static void liberarBiblioteca(void)
{
    if (g_hDll) {
        /* Libera a DLL da memória */
        FreeLibrary(g_hDll);
        /* Reseta o handle para NULL para indicar que não há mais DLL carregada */
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

/**
 * Permite ao usuário configurar os parâmetros de conexão com a impressora
 * 
 * Solicita:
 * - Tipo de conexão (USB, Serial, etc)
 * - Modelo da impressora
 * - Tipo/porta de conexão
 * - Parâmetro adicional
 * 
 * As configurações são armazenadas nas variáveis globais
 */
static void configurarConexao(void)
{
    printf("\n=== Configuracao de Conexao ===\n");
    
    /* Solicita o tipo de conexão (1=USB, 2=Serial, etc) */
    printf("Tipo (1=USB, 2=Serial, etc): ");
    scanf("%d", &g_tipo);
    flush_entrada(); /* Remove o '\n' deixado pelo scanf */
    
    /* Solicita o modelo da impressora (ex: "i9", "i7") */
    printf("Modelo (ex: i9): ");
    if (fgets(g_modelo, sizeof(g_modelo), stdin) != NULL) {
        size_t len = strlen(g_modelo);
        /* Remove o caractere de nova linha se existir */
        if (len > 0 && g_modelo[len-1] == '\n') {
            g_modelo[len-1] = '\0';
        }
    }
    
    /* Solicita o tipo ou porta de conexão (ex: "USB", "COM1") */
    printf("Conexao (ex: USB, COM1): ");
    if (fgets(g_conexao, sizeof(g_conexao), stdin) != NULL) {
        size_t len = strlen(g_conexao);
        /* Remove o caractere de nova linha se existir */
        if (len > 0 && g_conexao[len-1] == '\n') {
            g_conexao[len-1] = '\0';
        }
    }
    
    /* Solicita um parâmetro adicional de configuração */
    printf("Parametro: ");
    scanf("%d", &g_parametro);
    flush_entrada(); /* Remove o '\n' deixado pelo scanf */
    
    printf("Configuracao salva!\n");
}

/**
 * Abre a conexão com a impressora usando as configurações armazenadas
 * 
 * Verifica se já existe uma conexão aberta antes de tentar abrir uma nova
 * Retorna 0 em caso de sucesso, código de erro caso contrário
 */
static void abrirConexao(void)
{
    /* Verifica se já existe uma conexão aberta */
    if (g_conectada) {
        printf("Conexao ja esta aberta!\n");
        return;
    }
    
    /* Verifica se a função da DLL foi carregada corretamente */
    if (!AbreConexaoImpressora) {
        printf("Erro: Funcao AbreConexaoImpressora nao carregada!\n");
        return;
    }
    
    /* Chama a função da DLL para abrir a conexão */
    /* Parâmetros: tipo, modelo, conexão, parâmetro */
    int ret = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);
    if (ret == 0) {
        /* Sucesso: atualiza a flag de conexão */
        g_conectada = 1;
        printf("Conexao aberta com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro retornado pela DLL */
        printf("Erro ao abrir conexao (codigo: %d)\n", ret);
    }
}

/**
 * Fecha a conexão com a impressora
 * 
 * Verifica se existe uma conexão aberta antes de tentar fechar
 * Atualiza a flag g_conectada para 0 em caso de sucesso
 */
static void fecharConexao(void)
{
    /* Verifica se existe uma conexão aberta */
    if (!g_conectada) {
        printf("Nenhuma conexao aberta!\n");
        return;
    }
    
    /* Verifica se a função da DLL foi carregada corretamente */
    if (!FechaConexaoImpressora) {
        printf("Erro: Funcao FechaConexaoImpressora nao carregada!\n");
        return;
    }
    
    /* Chama a função da DLL para fechar a conexão */
    int ret = FechaConexaoImpressora();
    if (ret == 0) {
        /* Sucesso: atualiza a flag de conexão */
        g_conectada = 0;
        printf("Conexao fechada com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro retornado pela DLL */
        printf("Erro ao fechar conexao (codigo: %d)\n", ret);
    }
}

/**
 * Imprime um texto na impressora
 * 
 * Solicita o texto ao usuário, imprime, avança o papel e corta
 * Parâmetros da impressão: texto, alinhamento(0), fonte(0), tamanho(0)
 */
static void imprimirTexto(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Buffer para armazenar o texto a ser impresso (máximo 512 caracteres) */
    char texto[512];
    printf("\nDigite o texto a ser impresso: ");
    if (fgets(texto, sizeof(texto), stdin) != NULL) {
        size_t len = strlen(texto);
        /* Remove o caractere de nova linha se existir */
        if (len > 0 && texto[len-1] == '\n') {
            texto[len-1] = '\0';
        }
    }
    
    /* Verifica se as funções necessárias foram carregadas */
    if (!ImpressaoTexto || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        return;
    }
    
    /* Imprime o texto: texto, alinhamento, fonte, tamanho */
    int ret = ImpressaoTexto(texto, 0, 0, 0);
    if (ret == 0) {
        /* Sucesso: avança 10 linhas e corta o papel */
        AvancaPapel(10);
        Corte(0);
        printf("Texto impresso com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro */
        printf("Erro ao imprimir texto (codigo: %d)\n", ret);
    }
}

/**
 * Imprime um QR Code na impressora
 * 
 * Solicita o conteúdo do QR Code ao usuário
 * Parâmetros: conteúdo, tamanho(6), nível de correção(4)
 * Após imprimir, avança o papel e corta
 */
static void imprimirQRCode(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Buffer para armazenar o conteúdo do QR Code (máximo 256 caracteres) */
    char qrtexto[256];
    printf("\nDigite o conteudo do QR Code: ");
    if (fgets(qrtexto, sizeof(qrtexto), stdin) != NULL) {
        size_t len = strlen(qrtexto);
        /* Remove o caractere de nova linha se existir */
        if (len > 0 && qrtexto[len-1] == '\n') {
            qrtexto[len-1] = '\0';
        }
    }
    
    /* Verifica se as funções necessárias foram carregadas */
    if (!ImpressaoQRCode || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        return;
    }
    
    /* Imprime o QR Code: conteúdo, tamanho(6), correção de erro(4) */
    int ret = ImpressaoQRCode(qrtexto, 6, 4);
    if (ret == 0) {
        /* Sucesso: avança 10 linhas e corta o papel */
        AvancaPapel(10);
        Corte(0);
        printf("QR Code impresso com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro */
        printf("Erro ao imprimir QR Code (codigo: %d)\n", ret);
    }
}

/**
 * Imprime um código de barras na impressora
 * 
 * Usa valores fixos para demonstração:
 * - Tipo 8: Tipo de código de barras
 * - "{A012345678912": Dados do código (formato Code128)
 * - 100: Altura do código
 * - 2: Largura
 * - 3: Posição do texto
 * Após imprimir, avança o papel e corta
 */
static void imprimirCodigoBarras(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Verifica se as funções necessárias foram carregadas */
    if (!ImpressaoCodigoBarras || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        return;
    }
    
    /* Imprime o código de barras: tipo(8), dados, altura(100), largura(2), posição(3) */
    /* Nota: Os dados estão em formato Code128 (começam com "{A") */
    int ret = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    if (ret == 0) {
        /* Sucesso: avança 10 linhas e corta o papel */
        AvancaPapel(10);
        Corte(0);
        printf("Codigo de barras impresso com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro */
        printf("Erro ao imprimir codigo de barras (codigo: %d)\n", ret);
    }
}

/**
 * Imprime um XML do SAT (Sistema Autenticador e Transmissor de Cupons Fiscais)
 * 
 * Lê o arquivo XMLSAT.xml do diretório atual, carrega todo o conteúdo em memória
 * e envia para a impressora. Após imprimir, avança o papel e corta.
 */
static void imprimirXMLSAT(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Abre o arquivo XML no modo leitura */
    FILE *arquivo = fopen("./XMLSAT.xml", "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir o arquivo XMLSAT.xml\n");
        return;
    }
    
    /* Move o ponteiro do arquivo para o final para descobrir o tamanho */
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    /* Volta o ponteiro para o início para ler o arquivo */
    fseek(arquivo, 0, SEEK_SET);
    
    /* Aloca memória para armazenar todo o conteúdo do arquivo */
    /* +1 para o caractere nulo terminador */
    char *conteudo = (char*)malloc(tamanho + 1);
    if (!conteudo) {
        printf("Erro: Falha ao alocar memoria\n");
        fclose(arquivo);
        return;
    }
    
    /* Lê todo o conteúdo do arquivo de uma vez */
    fread(conteudo, 1, tamanho, arquivo);
    /* Adiciona o terminador nulo para formar uma string válida */
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    /* Verifica se as funções necessárias foram carregadas */
    if (!ImprimeXMLSAT || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        free(conteudo); /* Libera a memória antes de retornar */
        return;
    }
    
    /* Imprime o XML: conteúdo, tipo(0) */
    int ret = ImprimeXMLSAT(conteudo, 0);
    /* Libera a memória alocada para o conteúdo do arquivo */
    free(conteudo);
    
    if (ret == 0) {
        /* Sucesso: avança 10 linhas e corta o papel */
        AvancaPapel(10);
        Corte(0);
        printf("XML SAT impresso com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro */
        printf("Erro ao imprimir XML SAT (codigo: %d)\n", ret);
    }
}

/**
 * Imprime um XML de cancelamento do SAT
 * 
 * Lê o arquivo CANC_SAT.xml do diretório atual e imprime junto com uma assinatura digital
 * A assinatura é necessária para validar o cancelamento fiscal
 * Após imprimir, avança o papel e corta
 */
static void imprimirXMLCancelamentoSAT(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Abre o arquivo XML de cancelamento no modo leitura */
    FILE *arquivo = fopen("./CANC_SAT.xml", "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir o arquivo CANC_SAT.xml\n");
        return;
    }
    
    /* Move o ponteiro do arquivo para o final para descobrir o tamanho */
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    /* Volta o ponteiro para o início para ler o arquivo */
    fseek(arquivo, 0, SEEK_SET);
    
    /* Aloca memória para armazenar todo o conteúdo do arquivo */
    /* +1 para o caractere nulo terminador */
    char *conteudo = (char*)malloc(tamanho + 1);
    if (!conteudo) {
        printf("Erro: Falha ao alocar memoria\n");
        fclose(arquivo);
        return;
    }
    
    /* Lê todo o conteúdo do arquivo de uma vez */
    fread(conteudo, 1, tamanho, arquivo);
    /* Adiciona o terminador nulo para formar uma string válida */
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    /* Assinatura digital do cancelamento (em Base64) */
    /* Esta assinatura valida a autenticidade do cancelamento fiscal */
    const char *assinatura = 
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
    
    /* Verifica se as funções necessárias foram carregadas */
    if (!ImprimeXMLCancelamentoSAT || !AvancaPapel || !Corte) {
        printf("Erro: Funcoes nao carregadas!\n");
        free(conteudo); /* Libera a memória antes de retornar */
        return;
    }
    
    /* Imprime o XML de cancelamento: conteúdo, assinatura, tipo(0) */
    int ret = ImprimeXMLCancelamentoSAT(conteudo, assinatura, 0);
    /* Libera a memória alocada para o conteúdo do arquivo */
    free(conteudo);
    
    if (ret == 0) {
        /* Sucesso: avança 10 linhas e corta o papel */
        AvancaPapel(10);
        Corte(0);
        printf("XML Cancelamento SAT impresso com sucesso!\n");
    } else {
        /* Erro: exibe o código de erro */
        printf("Erro ao imprimir XML Cancelamento SAT (codigo: %d)\n", ret);
    }
}

/**
 * Abre a gaveta de dinheiro específica para impressoras Elgin
 * 
 * Parâmetros:
 * - 1: Pino de controle da gaveta
 * - 50: Tempo de ativação em milissegundos (tempo1)
 * - 50: Tempo de espera em milissegundos (tempo2)
 */
static void abrirGavetaElginOpc(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Verifica se a função da DLL foi carregada corretamente */
    if (!AbreGavetaElgin) {
        printf("Erro: Funcao AbreGavetaElgin nao carregada!\n");
        return;
    }
    
    /* Abre a gaveta: pino(1), tempo1(50ms), tempo2(50ms) */
    int ret = AbreGavetaElgin(1, 50, 50);
    if (ret == 0) {
        printf("Gaveta Elgin aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir gaveta Elgin (codigo: %d)\n", ret);
    }
}

/**
 * Abre a gaveta de dinheiro (função genérica)
 * 
 * Parâmetros:
 * - 1: Pino de controle da gaveta
 * - 5: Tempo de ativação em milissegundos (tempo1)
 * - 10: Tempo de espera em milissegundos (tempo2)
 * 
 * Esta função é mais genérica e pode funcionar com outras marcas de impressora
 */
static void abrirGavetaOpc(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Verifica se a função da DLL foi carregada corretamente */
    if (!AbreGaveta) {
        printf("Erro: Funcao AbreGaveta nao carregada!\n");
        return;
    }
    
    /* Abre a gaveta: pino(1), tempo1(5ms), tempo2(10ms) */
    int ret = AbreGaveta(1, 5, 10);
    if (ret == 0) {
        printf("Gaveta aberta com sucesso!\n");
    } else {
        printf("Erro ao abrir gaveta (codigo: %d)\n", ret);
    }
}

/**
 * Emite um sinal sonoro na impressora
 * 
 * Parâmetros:
 * - 4: Quantidade de bipes
 * - 50: Tempo que o sinal fica ligado (em milissegundos)
 * - 5: Tempo que o sinal fica desligado entre bipes (em milissegundos)
 * 
 * Útil para alertas ou confirmações sonoras
 */
static void emitirSinalSonoro(void)
{
    /* Verifica se a conexão está aberta */
    if (!g_conectada) {
        printf("Erro: Abra a conexao primeiro!\n");
        return;
    }
    
    /* Verifica se a função da DLL foi carregada corretamente */
    if (!SinalSonoro) {
        printf("Erro: Funcao SinalSonoro nao carregada!\n");
        return;
    }
    
    /* Emite o sinal sonoro: quantidade(4), tempo_on(50ms), tempo_off(5ms) */
    int ret = SinalSonoro(4, 50, 5);
    if (ret == 0) {
        printf("Sinal sonoro emitido com sucesso!\n");
    } else {
        printf("Erro ao emitir sinal sonoro (codigo: %d)\n", ret);
    }
}

/* ======================= FUNÇÃO PRINCIPAL ======================= */

/**
 * Função principal do programa
 * 
 * Fluxo de execução:
 * 1. Carrega a DLL e todas as funções necessárias
 * 2. Entra em um loop infinito exibindo o menu
 * 3. Processa a opção escolhida pelo usuário
 * 4. Ao sair (opção 0), fecha a conexão se aberta e libera a DLL
 */
int main(void)
{
    /* Tenta carregar a DLL e todas as funções */
    /* Se falhar, encerra o programa com código de erro */
    if (!carregarFuncoes()) {
        return 1;
    }

    /* Variável para armazenar a opção escolhida pelo usuário */
    int opcao = 0;
    
    /* Loop principal do programa - executa até o usuário escolher sair (0) */
    while (1) {
        /* Exibe o menu de opções */
        exibirMenu();
        /* Lê a opção escolhida pelo usuário */
        scanf("%d", &opcao);
        /* Limpa o buffer de entrada para evitar problemas na próxima leitura */
        flush_entrada();
        
        /* Processa a opção escolhida */
        switch (opcao) {
            case 1:
                /* Configura os parâmetros de conexão */
                configurarConexao();
                break;
            case 2:
                /* Abre a conexão com a impressora */
                abrirConexao();
                break;
            case 3:
                /* Fecha a conexão com a impressora */
                fecharConexao();
                break;
            case 4:
                /* Imprime um texto na impressora */
                imprimirTexto();
                break;
            case 5:
                /* Imprime um QR Code */
                imprimirQRCode();
                break;
            case 6:
                /* Imprime um código de barras */
                imprimirCodigoBarras();
                break;
            case 7:
                /* Imprime um XML do SAT */
                imprimirXMLSAT();
                break;
            case 8:
                /* Imprime um XML de cancelamento do SAT */
                imprimirXMLCancelamentoSAT();
                break;
            case 9:
                /* Abre a gaveta Elgin */
                abrirGavetaElginOpc();
                break;
            case 10:
                /* Abre a gaveta genérica */
                abrirGavetaOpc();
                break;
            case 11:
                /* Emite um sinal sonoro */
                emitirSinalSonoro();
                break;
            case 12:
                /* Inicializa a impressora */
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
                /* Opção de sair: encerra o programa */
                /* Se houver conexão aberta, fecha antes de sair */
                if (g_conectada) {
                    fecharConexao();
                }
                /* Libera a DLL da memória */
                liberarBiblioteca();
                printf("Encerrando programa...\n");
                return 0;
            default:
                /* Opção inválida: informa o usuário */
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    }
}
 