#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_CANDIDATES 5

sem_t booths[NUM_CANDIDATES];
sem_t room; // Semaphore to limit the number of candidates accessing booths at the same time

void* candidate(void* num) {
    int id = *(int*)num;

    while (1) {
        printf("Candidate %d is preparing to vote\n", id);
        sleep(1);

        sem_wait(&room); // Try to enter the room

        // Pick up left booth
        sem_wait(&booths[id]);
        printf("Candidate %d picked up booth %d (left)\n", id, id);

        // Pick up right booth
        sem_wait(&booths[(id + 1) % NUM_CANDIDATES]);
        printf("Candidate %d picked up booth %d (right)\n", id, (id + 1) % NUM_CANDIDATES);

        // Voting
        printf("Candidate %d is voting\n", id);
        sleep(1);

        // Put down right booth
        sem_post(&booths[(id + 1) % NUM_CANDIDATES]);
        printf("Candidate %d put down booth %d (right)\n", id, (id + 1) % NUM_CANDIDATES);

        // Put down left booth
        sem_post(&booths[id]);
        printf("Candidate %d put down booth %d (left)\n", id, id);

        sem_post(&room); // Leave the room

        printf("Candidate %d is done voting and preparing again\n", id);
    }
    return NULL;
}

int main() {
    pthread_t candidates[NUM_CANDIDATES];
    int ids[NUM_CANDIDATES];
   int i;
    // Initialize semaphores
    sem_init(&room, 0, NUM_CANDIDATES - 1); // Allow up to NUM_CANDIDATES-1 candidates to enter room
    for (i = 0; i < NUM_CANDIDATES; i++) {
        sem_init(&booths[i], 0, 1);
    }

    // Create candidate threads
    for (i = 0; i < NUM_CANDIDATES; i++) {
        ids[i] = i;
        pthread_create(&candidates[i], NULL, candidate, &ids[i]);
    }

    // Wait for candidate threads to finish (they never will in this example)
    for (i = 0; i < NUM_CANDIDATES; i++) {
        pthread_join(candidates[i], NULL);
    }

    // Destroy semaphores
    for (i = 0; i < NUM_CANDIDATES; i++) {
        sem_destroy(&booths[i]);
    }
    sem_destroy(&room);

    return 0;
}

