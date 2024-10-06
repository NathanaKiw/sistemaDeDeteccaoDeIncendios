#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "functions.h"

int main() {
    srand(time(NULL));

    inicializar_floresta();

    pthread_t sensores[TAMANHO][TAMANHO];
    pthread_t gerador, central, visualizador;

    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            if (rand() % 2 == 0) {
                floresta[i][j].estado = SENSOR;
                int* pos = malloc(2 * sizeof(int));
                pos[0] = i;
                pos[1] = j;
                pthread_create(&sensores[i][j], NULL, sensor, pos);
            }
        }
    }

    pthread_create(&gerador, NULL, gerador_de_incendios, NULL);
    pthread_create(&central, NULL, central_de_controle, NULL);
    pthread_create(&visualizador, NULL, visualizador_floresta, NULL);

    pthread_join(gerador, NULL);
    pthread_join(central, NULL);
    pthread_join(visualizador, NULL);

    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            if (floresta[i][j].estado == SENSOR) {
                pthread_join(sensores[i][j], NULL);
            }
        }
    }

    return 0;
}
