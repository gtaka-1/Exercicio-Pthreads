#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

double *A;
double *B;
double *C;

long long N;
int num_threads;

void *soma_vetores(void *arg) {
    long thread_id = *(long *)arg;

    long long inicio = thread_id * (N / num_threads);

    long long fim;

    if (thread_id == num_threads - 1)
        fim = N;
    else
        fim = (thread_id + 1) * (N / num_threads);

    for (long long i = inicio; i < fim; i++) {
        C[i] = A[i] + B[i];
    }

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
  
    A = malloc(N * sizeof(double));
    B = malloc(N * sizeof(double));
    C = malloc(N * sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    for (long long i = 0; i < N; i++) {
        A[i] = 1.0;
        B[i] = 2.0;
    }

    pthread_t *threads =
        malloc(num_threads * sizeof(pthread_t));

    long *ids =
        malloc(num_threads * sizeof(long));

    if (threads == NULL || ids == NULL) {
        printf("Erro ao alocar memoria para as threads.\n");
        return 1;
    }

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_threads; i++) {
        ids[i] = i;

        pthread_create(
            &threads[i],
            NULL,
            soma_vetores,
            &ids[i]
        );
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo =
        (fim.tv_sec - inicio.tv_sec) +
        (fim.tv_nsec - inicio.tv_nsec) / 1e9;
  
    printf("[Soma Vetores] N: %lld | Threads: %d | C[0]: %.2f | C[N-1]: %.2f | Tempo: %.4f s\n",
           N, num_threads, C[0], C[N - 1], tempo);

    free(A);
    free(B);
    free(C);
    free(threads);
    free(ids);

    return 0;
}
