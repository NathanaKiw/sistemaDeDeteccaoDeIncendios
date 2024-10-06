#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define TAMANHO 30
#define LIVRE '-'
#define SENSOR 'T'
#define FOGO '@'
#define QUEIMADO '/'

typedef struct {
    char estado;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int reportar_fogo;
} Celula;

extern Celula floresta[TAMANHO][TAMANHO];

void inicializar_floresta();
void* sensor(void* args);
void* gerador_de_incendios(void* args);
void* central_de_controle(void* args);
void* visualizador_floresta(void* args);
void imprimir_floresta();
void notificar_vizinhos(int x, int y);

#endif // FUNCTIONS_H
