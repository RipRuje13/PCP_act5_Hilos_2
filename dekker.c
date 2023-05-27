#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 10000   // Tamaño del arreglo
#define M 2000    // Tamaño de cada sección

int array[N];  // Arreglo de números aleatorios
int partial_sum[5];  // Arreglo para guardar las sumas parciales
int total_sum = 0;  // Variable compartida para la suma total

int turn = 0;  // Variable compartida para el algoritmo de Dekker
int interested[5] = {0, 0, 0, 0, 0};  // Variable compartida para el algoritmo de Dekker

void dekker_lock(int i) {
    int other = 1 - i;
    interested[i] = 1;
    while (interested[other]) {
        if (turn == other) {
            interested[i] = 0;
            while (turn == other) {
                // Espera activa (busy wait)
            }
            interested[i] = 1;
        }
    }
}

void dekker_unlock(int i) {
    turn = 1 - i;
    interested[i] = 0;
}

void* partial_sum_thread(void* arg) {
    int id = *((int*) arg);
    int start = id * M;
    int end = start + M;
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += array[i];
    }
    partial_sum[id] = sum;
    dekker_lock(id);
    total_sum += sum;
    dekker_unlock(id);
    return NULL;
}

int main() {
    // Inicializar el arreglo con números aleatorios
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        array[i] = rand() % 100;
    }

    // Crear hilos para las sumas parciales
    pthread_t threads[5];
    int ids[5];
    for (int i = 0; i < 5; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, partial_sum_thread, &ids[i]);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    // Imprimir resultados
    printf("Sumas parciales:\n");
    for (int i = 0; i < 5; i++) {
        printf("Hilo %d: %d\n", i, partial_sum[i]);
    }
    printf("Suma total: %d\n", total_sum);

    return 0;
} 