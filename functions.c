// functions.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "functions.h"

Celula floresta[TAMANHO][TAMANHO];

void inicializar_floresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j].estado = LIVRE;
            pthread_mutex_init(&floresta[i][j].lock, NULL);
            pthread_cond_init(&floresta[i][j].cond, NULL);
            floresta[i][j].reportar_fogo = 0;
        }
    }
}

void notificar_vizinhos(int x, int y) {
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx >= 0 && nx < TAMANHO && ny >= 0 && ny < TAMANHO) {
            pthread_mutex_lock(&floresta[nx][ny].lock);
            if (floresta[nx][ny].estado == SENSOR) {
                floresta[nx][ny].reportar_fogo = 1;
                pthread_cond_signal(&floresta[nx][ny].cond);
                printf("Notificacao enviada para o sensor [%d, %d]\n", nx, ny);
            }
            pthread_mutex_unlock(&floresta[nx][ny].lock);
        }
    }
}

void* sensor(void* args) {
    int x = ((int*)args)[0];
    int y = ((int*)args)[1];
    free(args);

    while (1) {
        pthread_mutex_lock(&floresta[x][y].lock);

        while (floresta[x][y].reportar_fogo == 0) {
            pthread_cond_wait(&floresta[x][y].cond, &floresta[x][y].lock);
        }

        if (floresta[x][y].estado == FOGO) {
            printf("Sensor [%d, %d] detectou fogo e notificou vizinhos.\n", x, y);
            notificar_vizinhos(x, y);
        }

        floresta[x][y].reportar_fogo = 0;
        pthread_mutex_unlock(&floresta[x][y].lock);
        sleep(1);
    }
}


void* gerador_de_incendios(void* args) {
    while (1) {
        sleep(4);

        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        pthread_mutex_lock(&floresta[x][y].lock);
        if (floresta[x][y].estado == LIVRE) {
            floresta[x][y].estado = FOGO;
            printf("Incendio iniciado em [%d, %d]\n", x, y);
            notificar_vizinhos(x, y);
        }
        pthread_mutex_unlock(&floresta[x][y].lock);
    }
}

void* central_de_controle(void* args) {
    while (1) {
        sleep(2);

        for (int i = 0; i < TAMANHO; i++) {
            for (int j = 0; j < TAMANHO; j++) {
                pthread_mutex_lock(&floresta[i][j].lock);

                if (floresta[i][j].estado == FOGO) {
                    printf("Central: Combatendo fogo em [%d, %d]\n", i, j);
                    floresta[i][j].estado = QUEIMADO;
                }

                pthread_mutex_unlock(&floresta[i][j].lock);
            }
        }
    }
}

void* visualizador_floresta(void* args) {
    while (1) {
        printf("\nEstado atual da floresta:\n");
        imprimir_floresta();
        sleep(2);
    }
}

void imprimir_floresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            pthread_mutex_lock(&floresta[i][j].lock);
            printf("%c ", floresta[i][j].estado);
            pthread_mutex_unlock(&floresta[i][j].lock);
        }
        printf("\n");
    }
}
