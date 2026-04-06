// race.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static long counter = 0;
static const int N = 1000000;

void* worker(void* arg) {
    (void)arg;
    for (int i = 0; i < N; i++) {
        counter++; // intentional race
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, worker, NULL) != 0) return 1;
    if (pthread_create(&t2, NULL, worker, NULL) != 0) return 1;

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("counter=%ld (expected %d)\n", counter, 2 * N);
    return 0;
}
