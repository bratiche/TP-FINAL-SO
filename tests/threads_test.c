#include <check.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define THREADS 10
#define LONG_WAIT_TIME 5
#define SHORT_WAIT_TIME 1

/**
 * Test de sincronizacion mediante semaforos.
 * Se crean N threads que se bloquean usando sleep() con dos posibles tiempos:
 * LONG_WAIT_TIME y SHORT_WAIT_TIME.
 * Los threads tienen que bloquearse y desbloquearse de a uno en el orden que fueron ejecutados.
 */

typedef struct {
    int id;
    unsigned int time;
} ThreadData;

sem_t semaphore;
int a = 0, b = 0;
int sleep_order[THREADS];
int wakeup_order[THREADS];

void * start_routine(void* data) {
    int id = ((ThreadData *) data)->id;
    unsigned int time = ((ThreadData *) data)->time;

    printf("Thread %d created, going to sleep...\n", id);
    sleep_order[a++] = id;

    sem_wait(&semaphore);
    sleep(time);
    sem_post(&semaphore);

    printf("Thread %d waking up...\n", id);
    wakeup_order[b++] = id;

    free(data);
    return NULL;
}

START_TEST(test_threads)
    pthread_t tid[THREADS];
    sem_init(&semaphore, 0, 1);

    for (int i = 0; i < THREADS; i++) {
        ThreadData * data = malloc(sizeof(*data));
        data->id = i + 1;
        data->time = (i + 1) % 2 == 0 ? LONG_WAIT_TIME : SHORT_WAIT_TIME;
        pthread_create(&tid[i], NULL, start_routine, data);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    ck_assert_uint_eq(a, b);
    ck_assert_uint_eq(b, THREADS);

    for (int i = 0; i < THREADS; i++) {
        ck_assert_uint_eq(sleep_order[i], wakeup_order[i]);
    }

END_TEST


Suite * suite(void) {
    Suite *s   = suite_create("threads");
    TCase *tc  = tcase_create("threads");

    tcase_set_timeout(tc, THREADS * LONG_WAIT_TIME);
    tcase_add_test(tc, test_threads);

    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    int number_failed;
    SRunner *sr  = srunner_create(suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
