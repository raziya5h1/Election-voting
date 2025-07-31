#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define NUM_PHILOSOPHERS 5

sem_t forks[NUM_PHILOSOPHERS];
sem_t room; // Semaphore to limit the number of philosophers accessing forks at the same time

void *philosopher(void *num) {
    int id = *(int *)num;

    while (1) {
        printf("Philosopher %d is thinking\n", id);
        sleep(1);

        sem_wait(&room); // Try to enter the room

        // Pick up left fork
        sem_wait(&forks[id]);
        printf("Philosopher %d picked up fork %d (left)\n", id, id);

        // Pick up right fork
        sem_wait(&forks[(id + 1) % NUM_PHILOSOPHERS]);
        printf("Philosopher %d picked up fork %d (right)\n", id, (id + 1) % NUM_PHILOSOPHERS);

        // Eating
        printf("Philosopher %d is eating\n", id);
        sleep(1);

        // Put down right fork
        sem_post(&forks[(id + 1) % NUM_PHILOSOPHERS]);
        printf("Philosopher %d put down fork %d (right)\n", id, (id + 1) % NUM_PHILOSOPHERS);

        // Put down left fork
        sem_post(&forks[id]);
        printf("Philosopher %d put down fork %d (left)\n", id, id);

        sem_post(&room); // Leave the room

        printf("Philosopher %d is thinking again\n", id);
    }
    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize semaphores
    sem_init(&room, 0, NUM_PHILOSOPHERS - 1); // Allow up to NUM_PHILOSOPHERS-1 philosophers to enter room
    int i;
	for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&forks[i], 0, 1);
    }

    // Create philosopher threads
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    // Wait for philosopher threads to finish (they never will in this example)
    for ( i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Destroy semaphores
    for ( i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&forks[i]);
    }
    sem_destroy(&room);

    return 0;
}

