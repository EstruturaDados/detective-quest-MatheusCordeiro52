#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- ESTRUTURAS DE DADOS ---

/**
 * Estrutura para os Cômodos da Mansão (Árvore Binária Simples)
 * Conceito do Nível Novato
 */
typedef struct Sala {
    char* nome;
    struct Sala* esq;
    struct Sala* dir;
} Sala;

/**
 * Estrutura para as Pistas Coletadas (Árvore Binária de Busca - BST)
 * Conceito do Nível Aventureiro
 */
typedef struct PistaNode {
    char* pista;
    struct PistaNode* esq;
    struct PistaNode* dir;
} PistaNode;

/**
 * Estrutura para a Tabela Hash (Pista -> Suspeito)
 * Conceito do Nível Mestre
 */
#define HASH_SIZE 10
typedef struct HashItem {
    char* pista;     // Chave
    char* suspeito;  // Valor
    struct HashItem* next; // Para tratar colisões (encadeamento)
} HashItem;

// --- VARIÁVEIS GLOBAIS ---
PistaNode* arvorePistas = NULL;     // Raiz da BST de pistas coletadas
HashItem* tabelaHash[HASH_SIZE]; // Tabela hash para Pista->Suspeito

// --- FUNÇÕES DA ÁRVORE DA MANSÃO (NÍVEL NOVATO / MESTRE) ---

/**
 * criarSala() - cria dinamicamente um cômodo.
 * Aloca memória para uma nova sala e define seu nome.
 */
Sala* criarSala(char* nome) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    // Aloca memória e copia o nome
    novaSala->nome = (char*)malloc(strlen(nome) + 1);
    if (novaSala->nome == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esq = NULL;
    novaSala->dir = NULL;
    return novaSala;
}

// --- FUNÇÕES DA ÁRVORE DE PISTAS (BST) (NÍVEL AVENTUREIRO / MESTRE) ---

/**
 * inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas (BST).
 * Insere de forma ordenada (alfabética).
 */
PistaNode* inserirPista(PistaNode* raiz, char* pista) {
    // Caso base: Árvore vazia, cria um novo nó
    if (raiz == NULL) {
        PistaNode* novoNo = (PistaNode*)malloc(sizeof(PistaNode));
        novoNo->pista = (char*)malloc(strlen(pista) + 1);
        strcpy(novoNo->pista, pista);
        novoNo->esq = NULL;
        novoNo->dir = NULL;
        return novoNo;
    }

    // Compara a pista nova com a raiz atual
    int cmp = strcmp(pista, raiz->pista);

    // Evita duplicatas
    if (cmp == 0) {
        return raiz;
    }
    // Se for menor, insere à esquerda
    else if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    }
    // Se for maior, insere à direita
    else {
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    return raiz;
}

/**
 * exibirPistas() (Nível Aventureiro) - imprime a árvore de pistas em ordem alfabética.
 * Faz um percurso em-ordem (in-order) na BST.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esq);        // Visita subárvore esquerda
        printf("- %s\n", raiz->pista); // Visita raiz
        exibirPistas(raiz->dir);       // Visita subárvore direita
    }
}

// --- FUNÇÕES DA TABELA HASH (NÍVEL MESTRE) ---

/**
 * Função hash simples: soma os valores ASCII dos caracteres da pista.
 */
unsigned int hashFunction(char* pista) {
    unsigned long hash = 0;
    int c;
    while ((c = *pista++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % HASH_SIZE;
}

/**
 * inserirNaHash() – insere associação pista/suspeito na tabela hash.
 */
void inserirNaHash(char* pista, char* suspeito) {
    HashItem* novoItem = (HashItem*)malloc(sizeof(HashItem));
    novoItem->pista = (char*)malloc(strlen(pista) + 1);
    novoItem->suspeito = (char*)malloc(strlen(suspeito) + 1);
    strcpy(novoItem->pista, pista);
    strcpy(novoItem->suspeito, suspeito);
    novoItem->next = NULL;

    unsigned int indice = hashFunction(pista);

    // Adiciona no início da lista encadeada (tratamento de colisão)
    if (tabelaHash[indice] == NULL) {
        tabelaHash[indice] = novoItem;
    } else {
        novoItem->next = tabelaHash[indice];
        tabelaHash[indice] = novoItem;
    }
}

/**
 * encontrarSuspeito() – consulta o suspeito correspondente a uma pista.
 * Retorna o nome do suspeito ou NULL se a pista não for encontrada.
 */
char* encontrarSuspeito(char* pista) {
    unsigned int indice = hashFunction(pista);
    HashItem* item = tabelaHash[indice];

    // Percorre a lista encadeada no índice
    while (item != NULL) {
        if (strcmp(item->pista, pista) == 0) {
            return item->suspeito;
        }
        item = item->next;
    }
    return NULL; // Não encontrou
}

// --- FUNÇÕES DE LÓGICA DO JOGO (TODOS OS NÍVEIS) ---

/**
 * Lógica de Pista Estática (Requisito Nível Mestre)
 * Retorna a pista associada a uma sala específica.
 */
char* obterPistaDaSala(char* nomeSala) {
    if (strcmp(nomeSala, "Biblioteca") == 0) return "Carta rasgada";
    if (strcmp(nomeSala, "Cozinha") == 0) return "Frasco de veneno";
    if (strcmp(nomeSala, "Jardim de Inverno") == 0) return "Botina suja de terra";
    if (strcmp(nomeSala, "Escritorio") == 0) return "Chave mestra";
    if (strcmp(nomeSala, "Sala de Jantar") == 0) return "Agenda misteriosa";
    if (strcmp(nomeSala, "Sala de Estar") == 0) return "Pá com terra molhada"; // Pista bônus
    return NULL; // Sala sem pista
}

/**
 * explorarSalas() – navega pela árvore e ativa o sistema de pistas.
 * Controla a navegação do jogador (e, d, s) e coleta pistas.
 */
void explorarSalas(Sala* salaAtual) {
    if (salaAtual == NULL) {
        printf("Voce bateu em uma parede e voltou...\n");
        return;
    }

    char escolha;
    while (1) {
        printf("\n====================================\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // Lógica de Coleta de Pistas (Nível Aventureiro/Mestre)
        char* pistaEncontrada = obterPistaDaSala(salaAtual->nome);
        if (pistaEncontrada != NULL) {
            // Verifica se a pista já foi coletada
            int cmp = (arvorePistas == NULL) ? -1 : strcmp(pistaEncontrada, arvorePistas->pista); // Simulação de busca
            
            // Tenta inserir. Se já existir, a BST não insere duplicata.
            // Para feedback, precisaríamos de uma função 'buscar' separada.
            // Por simplicidade, apenas inserimos.
            arvorePistas = inserirPista(arvorePistas, pistaEncontrada);
            printf("PISTA ENCONTRADA: %s\n", pistaEncontrada);
        } else {
            printf("Nenhuma pista encontrada aqui.\n");
        }
        
        // Lógica de Navegação (Nível Novato)
        // O Nível Novato terminava no nó folha. Os níveis Aventureiro/Mestre
        // permitem sair (s) a qualquer momento.
        
        printf("\nEscolha seu caminho:\n");
        if (salaAtual->esq) printf(" (e) Esquerda -> %s\n", salaAtual->esq->nome);
        if (salaAtual->dir) printf(" (d) Direita  -> %s\n", salaAtual->dir->nome);
        printf(" (s) Sair da mansao\n");
        printf("Opcao: ");
        
        scanf(" %c", &escolha);
        while(getchar() != '\n'); // Limpa o buffer de entrada

        if (escolha == 'e') {
            if (salaAtual->esq != NULL) {
                salaAtual = salaAtual->esq;
            } else {
                printf("Caminho bloqueado.\n");
            }
        } else if (escolha == 'd') {
            if (salaAtual->dir != NULL) {
                salaAtual = salaAtual->dir;
            } else {
                printf("Caminho bloqueado.\n");
            }
        } else if (escolha == 's') {
            printf("Voce decide parar a exploracao por enquanto...\n");
            return; // Sai da função de exploração
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }
}

/**
 * Função auxiliar recursiva para contar pistas de um suspeito na BST.
 */
void contarPistasPorSuspeito(PistaNode* raiz, char* suspeitoAcusado, int* contador) {
    if (raiz == NULL) {
        return;
    }
    // Percurso em-ordem
    contarPistasPorSuspeito(raiz->esq, suspeitoAcusado, contador);

    // Verifica o suspeito da pista atual
    char* suspeitoDaPista = encontrarSuspeito(raiz->pista);
    if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeitoAcusado) == 0) {
        (*contador)++; // Incrementa o ponteiro do contador
    }

    contarPistasPorSuspeito(raiz->dir, suspeitoAcusado, contador);
}

/**
 * verificarSuspeitoFinal() – conduz à fase de julgamento final.
 * Pede a acusação e verifica na Tabela Hash usando as pistas da BST.
 */
void verificarSuspeitoFinal() {
    printf("\n====================================\n");
    printf("       FASE DE JULGAMENTO\n");
    printf("====================================\n");

    if (arvorePistas == NULL) {
        printf("Voce nao coletou nenhuma pista. Impossivel acusar alguem.\n");
        return;
    }

    printf("Pistas coletadas em ordem alfabetica:\n");
    exibirPistas(arvorePistas);
    
    printf("\n--- Suspeitos ---\n");
    printf("1. Mordomo\n");
    printf("2. Jardineiro\n");
    printf("3. Governanta\n");
    
    char acusacao[50];
    printf("\nQuem e o culpado? ");
    scanf("%49s", acusacao);
    while(getchar() != '\n'); // Limpa o buffer

    // Capitaliza a primeira letra para bater com a Hash (ex: "mordomo" -> "Mordomo")
    if (acusacao[0] >= 'a' && acusacao[0] <= 'z') {
        acusacao[0] = acusacao[0] - 32;
    }

    printf("\nAnalisando as pistas contra %s...\n", acusacao);

    int pistasContraAcusado = 0;
    // Usa a função recursiva para contar as pistas na BST
    contarPistasPorSuspeito(arvorePistas, acusacao, &pistasContraAcusado);

    printf("Voce tem %d pista(s) que apontam para %s.\n", pistasContraAcusado, acusacao);

    // Requisito: Pelo menos 2 pistas
    if (pistasContraAcusado >= 2) {
        printf("\nVEREDITO: CULPADO!\n");
        printf("As evidencias sao contundentes. %s foi preso!\n", acusacao);
        printf("Voce venceu o Detective Quest!\n");
    } else {
        printf("\nVEREDITO: INOCENTE!\n");
        printf("Voce nao tinha provas suficientes. O verdadeiro culpado escapou...\n");
        printf("Fim de jogo.\n");
    }
}

// --- FUNÇÕES DE LIMPEZA DE MEMÓRIA (BOA PRÁTICA) ---

void freeArvoreSalas(Sala* raiz) {
    if (raiz == NULL) return;
    freeArvoreSalas(raiz->esq);
    freeArvoreSalas(raiz->dir);
    free(raiz->nome);
    free(raiz);
}

void freeArvorePistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    freeArvorePistas(raiz->esq);
    freeArvorePistas(raiz->dir);
    free(raiz->pista);
    free(raiz);
}

void freeTabelaHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashItem* item = tabelaHash[i];
        while (item != NULL) {
            HashItem* temp = item;
            item = item->next;
            free(temp->pista);
            free(temp->suspeito);
            free(temp);
        }
    }
}


// --- FUNÇÃO PRINCIPAL ---

/**
 * main() – monta o mapa inicial e dá início à exploração.
 */
int main() {
    // 1. Inicializa a Tabela Hash (Nível Mestre)
    for (int i = 0; i < HASH_SIZE; i++) {
        tabelaHash[i] = NULL;
    }
    
    // 2. Monta o Mapa da Mansão (Árvore Binária) (Nível Novato)
    Sala* hall = criarSala("Hall de Entrada");
    hall->esq = criarSala("Sala de Estar");
    hall->dir = criarSala("Biblioteca");
    
    hall->esq->esq = criarSala("Cozinha");
    hall->esq->dir = criarSala("Sala de Jantar");
    
    hall->dir->esq = criarSala("Escritorio");
    hall->dir->dir = criarSala("Jardim de Inverno");
    
    // 3. Define as Pistas e Suspeitos (Tabela Hash) (Nível Mestre)
    // Pistas do Mordomo
    inserirNaHash("Carta rasgada", "Mordomo");
    inserirNaHash("Chave mestra", "Mordomo");
    
    // Pistas da Governanta
    inserirNaHash("Frasco de veneno", "Governanta");
    inserirNaHash("Agenda misteriosa", "Governanta");
    
    // Pistas do Jardineiro
    inserirNaHash("Botina suja de terra", "Jardineiro");
    inserirNaHash("Pá com terra molhada", "Jardineiro");

    // 4. Inicia o Jogo
    printf("--- BEM-VINDO AO DETECTIVE QUEST: NIVEL MESTRE ---\n");
    printf("Explore a mansao, colete as pistas e descubra o culpado.\n");
    
    // Fase de Exploração (Nível Novato/Aventureiro)
    explorarSalas(hall);
    
    // Fase de Julgamento (Nível Mestre)
    verificarSuspeitoFinal();
    
    // 5. Limpa toda a memória alocada
    freeArvoreSalas(hall);
    freeArvorePistas(arvorePistas);
    freeTabelaHash();
    
    return 0;
}
