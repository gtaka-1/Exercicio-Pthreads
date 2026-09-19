#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long long N;
int num_threads;

long long pontos_circulo = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *calcular_pi(void *arg) {
    long thread_id = *(long *)arg;

    long long pontos_por_thread = N / num_threads;

    long long inicio = thread_id * pontos_por_thread;

    long long fim = (thread_id == num_threads - 1)
                    ? N
                    : inicio + pontos_por_thread;

    long long contador_local = 0;

    unsigned int seed = (unsigned int)time(NULL) + thread_id;

    for (long long i = inicio; i < fim; i++) {

        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            contador_local++;
        }
    }
    pthread_mutex_lock(&mutex);
    pontos_circulo += contador_local;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Uso: %s <N> <num_threads>\n", argv[0]);
        return 1;
    }

    N = atoll(argv[1]);
    num_threads = atoi(argv[2]);

    if (N <= 0 || num_threads <= 0) {
        printf("Valores invalidos.\n");
        return 1;
    }

    pthread_t *threads =
        malloc(num_threads * sizeof(pthread_t));

    long *ids =
        malloc(num_threads * sizeof(long));

    if (threads == NULL || ids == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_threads; i++) {
        ids[i] = i;

        pthread_create(
            &threads[i],
            NULL,
            calcular_pi,
            &ids[i]
        );
    }
  
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double pi = 4.0 * pontos_circulo / N;

    double tempo =
        (fim.tv_sec - inicio.tv_sec) +
        (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf(
        "[Monte Carlo] N: %lld | Threads: %d | Pi: %.10f | Tempo: %.4f s\n",
        N,
        num_threads,
        pi,
        tempo
    );

    pthread_mutex_destroy(&mutex);

    free(threads);
    free(ids);

    return 0;
}
