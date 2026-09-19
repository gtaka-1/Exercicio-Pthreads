#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

long K;
int num_threads;

long total_primos = 0;
pthread_mutex_t mutex_primos = PTHREAD_MUTEX_INITIALIZER;

int eh_primo(long n) {
    if (n < 2)
        return 0;

    if (n == 2)
        return 1;

    if (n % 2 == 0)
        return 0;

    for (long i = 3; i <= sqrt((double)n); i += 2) {
        if (n % i == 0)
            return 0;
    }

    return 1;
}

void *contar_primos(void *arg) {
    long thread_id = *(long *)arg;
    long contador_local = 0;

    for (long n = 2 + thread_id; n <= K; n += num_threads) {
        if (eh_primo(n)) {
            contador_local++;
        }
    }

    pthread_mutex_lock(&mutex_primos);
    total_primos += contador_local;
    pthread_mutex_unlock(&mutex_primos);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <K> <num_threads>\n", argv[0]);
        return 1;
    }

    K = atol(argv[1]);
    num_threads = atoi(argv[2]);

    if (K < 2 || num_threads < 1) {
        printf("Valores invalidos.\n");
        return 1;
    }

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    long *ids = malloc(num_threads * sizeof(long));

    if (threads == NULL || ids == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_threads; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, contar_primos, &ids[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo =
        (fim.tv_sec - inicio.tv_sec) +
        (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("[Primos] K: %ld | Threads: %d | Primos: %ld | Tempo: %.4f s\n",
           K, num_threads, total_primos, tempo);

    pthread_mutex_destroy(&mutex_primos);
    free(threads);
    free(ids);

    return 0;
}
