#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 51
#define TAM_DESTINO 51
#define NUM_CATEGORIAS 3
#define CAPACIDADE_INICIAL 2
#define ARQUIVO_DADOS "viagens.txt"

/*
    Categorias de gastos:
    0 - Hospedagem
    1 - Transporte
    2 - Alimentacao
*/
const char *CATEGORIAS[NUM_CATEGORIAS] = {
    "Hospedagem",
    "Transporte",
    "Alimentacao"
};

/*
    Struct principal que representa uma viagem.

    - nome: nome que voce quiser dar para a viagem (ex: "Viagem de Fim de Ano")
    - destino: cidade/país (ex: "Rio de Janeiro", "Lisboa")
    - dias: quantidade de dias da viagem
    - gastos: matriz dinamica dias x NUM_CATEGORIAS
              gastos[dia][categoria] = valor gasto
*/
typedef struct {
    char nome[TAM_NOME];
    char destino[TAM_DESTINO];
    int dias;
    float **gastos; /* matriz dinamica [dias][NUM_CATEGORIAS] */
} Viagem;

/*
    Struct gerenciador, que armazena um vetor dinamico de viagens.

    - lista: ponteiro para um array dinamico de Viagem
    - quantidade: quantas viagens estao em uso
    - capacidade: tamanho alocado do vetor lista
*/
typedef struct {
    Viagem *lista;
    int quantidade;
    int capacidade;
} GerenciadorViagens;

/* === Funcoes auxiliares de entrada === */

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta ate o fim da linha */
    }
}

void remover_nova_linha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/* Le um inteiro com validacao de faixa */
int ler_inteiro(const char *mensagem, int minimo, int maximo) {
    int valor;
    int ok = 0;

    while (!ok) {
        printf("%s", mensagem);
        if (scanf("%d", &valor) != 1) {
            printf("Entrada invalida. Tente novamente.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (valor < minimo || valor > maximo) {
            printf("Valor fora da faixa (%d a %d). Tente novamente.\n", minimo, maximo);
        } else {
            ok = 1;
        }
    }
    return valor;
}

/* Le um float com validacao de minimo (pode ser 0.0) */
float ler_float(const char *mensagem, float minimo) {
    float valor;
    int ok = 0;

    while (!ok) {
        printf("%s", mensagem);
        if (scanf("%f", &valor) != 1) {
            printf("Entrada invalida. Tente novamente.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer();
        if (valor < minimo) {
            printf("Valor deve ser maior ou igual a %.2f. Tente novamente.\n", minimo);
        } else {
            ok = 1;
        }
    }
    return valor;
}

/* === Gerenciamento do vetor dinamico de viagens === */

void inicializar_gerenciador(GerenciadorViagens *g) {
    g->quantidade = 0;
    g->capacidade = CAPACIDADE_INICIAL;
    g->lista = (Viagem *) malloc(g->capacidade * sizeof(Viagem));
    if (g->lista == NULL) {
        printf("Erro de memoria ao inicializar gerenciador.\n");
        exit(EXIT_FAILURE);
    }
}

/*
    Garante que o vetor lista tenha espaco para pelo menos mais uma viagem.
    Se estiver cheio, dobra a capacidade usando realloc.
*/
int garantir_capacidade(GerenciadorViagens *g) {
    if (g->quantidade < g->capacidade) {
        return 1;
    }

    int nova_capacidade = g->capacidade * 2;
    Viagem *novo = (Viagem *) realloc(g->lista, nova_capacidade * sizeof(Viagem));
    if (novo == NULL) {
        printf("Erro de memoria ao realocar vetor de viagens.\n");
        return 0;
    }

    g->lista = novo;
    g->capacidade = nova_capacidade;
    return 1;
}

/*
    Libera toda a memoria dinamica associada ao gerenciador:
    - cada linha da matriz gastos de cada viagem
    - a matriz gastos em si
    - o vetor lista
*/
void liberar_gerenciador(GerenciadorViagens *g) {
    for (int i = 0; i < g->quantidade; i++) {
        Viagem *v = &g->lista[i];
        if (v->gastos != NULL) {
            for (int d = 0; d < v->dias; d++) {
                free(v->gastos[d]);
            }
            free(v->gastos);
        }
    }
    free(g->lista);
    g->lista = NULL;
    g->quantidade = 0;
    g->capacidade = 0;
}

/*
    Calcula o total de gastos de uma viagem somando toda a matriz gastos.
*/
float calcular_total_viagem(const Viagem *v) {
    float total = 0.0f;
    for (int d = 0; d < v->dias; d++) {
        for (int c = 0; c < NUM_CATEGORIAS; c++) {
            total += v->gastos[d][c];
        }
    }
    return total;
}

/* === Persistencia em arquivo texto === */

/*
    Formato do arquivo viagens.txt:

    linha 1: quantidade de viagens (int)
    para cada viagem:
        linha: nome
        linha: destino
        linha: dias (int)
        proximas 'dias' linhas:
            NUM_CATEGORIAS floats por linha (gastos daquele dia)
*/
void salvar_arquivo(const GerenciadorViagens *g, const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "w");
    if (f == NULL) {
        printf("Nao foi possivel abrir o arquivo %s para escrita.\n", nome_arquivo);
        return;
    }

    fprintf(f, "%d\n", g->quantidade);

    for (int i = 0; i < g->quantidade; i++) {
        const Viagem *v = &g->lista[i];
        fprintf(f, "%s\n", v->nome);
        fprintf(f, "%s\n", v->destino);
        fprintf(f, "%d\n", v->dias);
        for (int d = 0; d < v->dias; d++) {
            for (int c = 0; c < NUM_CATEGORIAS; c++) {
                fprintf(f, "%.2f", v->gastos[d][c]);
                if (c < NUM_CATEGORIAS - 1) {
                    fprintf(f, " ");
                }
            }
            fprintf(f, "\n");
        }
    }

    fclose(f);
    printf("Dados salvos em %s com sucesso.\n", nome_arquivo);
}

/*
    Carrega os dados do arquivo, se existir.
    Caso o arquivo nao exista ou esteja vazio/corrompido, o programa segue com lista vazia.
*/
void carregar_arquivo(GerenciadorViagens *g, const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "r");
    if (f == NULL) {
        /* Primeiro uso: sem arquivo ainda. Nao e erro. */
        return;
    }

    int qtd;
    if (fscanf(f, "%d\n", &qtd) != 1 || qtd <= 0) {
        fclose(f);
        return;
    }

    for (int i = 0; i < qtd; i++) {
        if (!garantir_capacidade(g)) {
            printf("Parando carga por falta de memoria.\n");
            break;
        }

        Viagem *v = &g->lista[g->quantidade];

        if (fgets(v->nome, TAM_NOME, f) == NULL) {
            break;
        }
        remover_nova_linha(v->nome);

        if (fgets(v->destino, TAM_DESTINO, f) == NULL) {
            break;
        }
        remover_nova_linha(v->destino);

        if (fscanf(f, "%d\n", &v->dias) != 1 || v->dias <= 0) {
            break;
        }

        v->gastos = (float **) malloc(v->dias * sizeof(float *));
        if (v->gastos == NULL) {
            printf("Erro de memoria ao carregar matriz de gastos.\n");
            break;
        }

        for (int d = 0; d < v->dias; d++) {
            v->gastos[d] = (float *) malloc(NUM_CATEGORIAS * sizeof(float));
            if (v->gastos[d] == NULL) {
                printf("Erro de memoria ao carregar linha da matriz.\n");
                /* em caso de erro, liberacao parcial */
                for (int k = 0; k < d; k++) {
                    free(v->gastos[k]);
                }
                free(v->gastos);
                v->gastos = NULL;
                fclose(f);
                return;
            }
            for (int c = 0; c < NUM_CATEGORIAS; c++) {
                if (fscanf(f, "%f", &v->gastos[d][c]) != 1) {
                    v->gastos[d][c] = 0.0f;
                }
            }
            /* consome o fim da linha */
            int ch;
            while ((ch = fgetc(f)) != '\n' && ch != EOF) {
                /* descarta */
            }
        }

        g->quantidade++;
    }

    fclose(f);
}

/* === Funcoes de operacao do menu === */

/*
    Adiciona uma nova viagem lendo dados do usuario
    e criando a matriz dinamica de gastos (inicializada com 0).
*/
void adicionar_viagem(GerenciadorViagens *g) {
    if (!garantir_capacidade(g)) {
        return;
    }

    Viagem *v = &g->lista[g->quantidade];

    printf("\n=== Cadastro de nova viagem ===\n");
    printf("Nome da viagem: ");
    fgets(v->nome, TAM_NOME, stdin);
    remover_nova_linha(v->nome);

    printf("Destino: ");
    fgets(v->destino, TAM_DESTINO, stdin);
    remover_nova_linha(v->destino);

    v->dias = ler_inteiro("Quantidade de dias da viagem (1 a 365): ", 1, 365);

    /* Alocacao dinamica da matriz de gastos: dias x NUM_CATEGORIAS */
    v->gastos = (float **) malloc(v->dias * sizeof(float *));
    if (v->gastos == NULL) {
        printf("Erro de memoria ao alocar matriz de gastos.\n");
        return;
    }

    for (int d = 0; d < v->dias; d++) {
        v->gastos[d] = (float *) calloc(NUM_CATEGORIAS, sizeof(float));
        if (v->gastos[d] == NULL) {
            printf("Erro de memoria ao alocar linha da matriz.\n");
            /* libera linhas ja alocadas */
            for (int k = 0; k < d; k++) {
                free(v->gastos[k]);
            }
            free(v->gastos);
            v->gastos = NULL;
            return;
        }
    }

    g->quantidade++;
    printf("Viagem cadastrada com sucesso!\n");
}

/*
    Permite ao usuario escolher um indice de viagem (1..quantidade).
    Retorna o indice real (0..quantidade-1) ou -1 se nao houver viagens.
*/
int escolher_viagem(const GerenciadorViagens *g) {
    if (g->quantidade == 0) {
        printf("Nenhuma viagem cadastrada ainda.\n");
        return -1;
    }

    printf("\n=== Viagens cadastradas ===\n");
    for (int i = 0; i < g->quantidade; i++) {
        printf("%d) %s (%s) - %d dias\n",
               i + 1,
               g->lista[i].nome,
               g->lista[i].destino,
               g->lista[i].dias);
    }

    int opcao = ler_inteiro("Escolha o numero da viagem: ", 1, g->quantidade);
    return opcao - 1;
}

/*
    Registra um gasto em uma determinada viagem, dia e categoria.
    O valor informado e somado ao valor existente naquele dia/categoria.
*/
void registrar_gasto(GerenciadorViagens *g) {
    int idx = escolher_viagem(g);
    if (idx < 0) {
        return;
    }

    Viagem *v = &g->lista[idx];

    printf("\n=== Registrar gasto para a viagem \"%s\" (%s) ===\n",
           v->nome, v->destino);

    int dia = ler_inteiro("Dia da viagem (1 a quantidade de dias): ", 1, v->dias);

    printf("\nCategorias de gasto:\n");
    for (int c = 0; c < NUM_CATEGORIAS; c++) {
        printf("%d) %s\n", c + 1, CATEGORIAS[c]);
    }

    int cat = ler_inteiro("Escolha a categoria: ", 1, NUM_CATEGORIAS);

    float valor = ler_float("Valor do gasto (>= 0): ", 0.0f);

    v->gastos[dia - 1][cat - 1] += valor;

    printf("Gasto registrado com sucesso! Total acumulado neste dia/categoria: R$ %.2f\n",
           v->gastos[dia - 1][cat - 1]);
}

/*
    Lista todas as viagens com o total geral de gastos de cada uma.
*/
void listar_viagens(const GerenciadorViagens *g) {
    if (g->quantidade == 0) {
        printf("Nenhuma viagem cadastrada ainda.\n");
        return;
    }

    printf("\n=== Lista de viagens e totais ===\n");
    for (int i = 0; i < g->quantidade; i++) {
        const Viagem *v = &g->lista[i];
        float total = calcular_total_viagem(v);
        printf("%d) %s (%s) - %d dias - Total: R$ %.2f\n",
               i + 1, v->nome, v->destino, v->dias, total);
    }
}

/*
    Mostra a matriz completa de gastos de uma viagem:
    linhas = dias, colunas = categorias.
*/
void mostrar_matriz_gastos(const GerenciadorViagens *g) {
    int idx = escolher_viagem(g);
    if (idx < 0) {
        return;
    }

    const Viagem *v = &g->lista[idx];

    printf("\n=== Matriz de gastos da viagem \"%s\" (%s) ===\n",
           v->nome, v->destino);

    /* cabecalho das categorias */
    printf("Dia ");
    for (int c = 0; c < NUM_CATEGORIAS; c++) {
        printf(" | %-11s", CATEGORIAS[c]);
    }
    printf(" | Total do dia\n");
    printf("-------------------------------------------------------------\n");

    for (int d = 0; d < v->dias; d++) {
        float total_dia = 0.0f;
        printf("%3d ", d + 1);
        for (int c = 0; c < NUM_CATEGORIAS; c++) {
            printf(" | %11.2f", v->gastos[d][c]);
            total_dia += v->gastos[d][c];
        }
        printf(" | %11.2f\n", total_dia);
    }

    float total_geral = calcular_total_viagem(v);
    printf("-------------------------------------------------------------\n");
    printf("Total geral da viagem: R$ %.2f\n", total_geral);
}

/* === Menu principal === */

void mostrar_menu() {
    printf("\n=== Planejador de Viagens ===\n");
    printf("1) Cadastrar nova viagem\n");
    printf("2) Registrar gasto em uma viagem\n");
    printf("3) Listar viagens e totais\n");
    printf("4) Mostrar matriz de gastos de uma viagem\n");
    printf("5) Salvar e sair\n");
}

int main(void) {
    GerenciadorViagens gerenciador;
    inicializar_gerenciador(&gerenciador);

    /* Carrega dados anteriores, se existirem */
    carregar_arquivo(&gerenciador, ARQUIVO_DADOS);

    int opcao;
    do {
        mostrar_menu();
        opcao = ler_inteiro("Escolha uma opcao: ", 1, 5);

        switch (opcao) {
            case 1:
                adicionar_viagem(&gerenciador);
                break;
            case 2:
                registrar_gasto(&gerenciador);
                break;
            case 3:
                listar_viagens(&gerenciador);
                break;
            case 4:
                mostrar_matriz_gastos(&gerenciador);
                break;
            case 5:
                salvar_arquivo(&gerenciador, ARQUIVO_DADOS);
                printf("Saindo do programa. Ate a proxima viagem!\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 5);

    liberar_gerenciador(&gerenciador);
    return 0;
}
