#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_POLITICIANS 5
#define NUM_AREAS 10

sem_t booths[NUM_AREAS]; // Semaphores for voting booths in different areas
sem_t room; // Semaphore to limit the number of candidates voting at the same time

int votes[NUM_POLITICIANS] = {0}; // Votes received by each candidate
pthread_mutex_t vote_lock; // Mutex to protect vote counting

void* politician(void* num) {
    int id = *(int*)num;

    printf("Politician %d is preparing to vote\n", id);
    sleep(1);

    sem_wait(&room); // Try to enter the voting room

    int area = rand() % NUM_AREAS; // Randomly choose an area for the politician to vote
    sem_wait(&booths[area]);
    printf("Politician %d entered booth in area %d\n", id, area);

    // Voting
    printf("Politician %d is voting in area %d\n", id, area);
    sleep(1);

    // Lock the votes array and update the vote count
    pthread_mutex_lock(&vote_lock);
    votes[id]++; // Increment the vote count for this politician
    pthread_mutex_unlock(&vote_lock);

    // Leave the booth
    sem_post(&booths[area]);
    printf("Politician %d left booth in area %d\n", id, area);

    sem_post(&room); // Leave the voting room

    printf("Politician %d is done voting and preparing again\n", id);

    // Exit after casting a vote for simplicity in this example
    return NULL;
}

int main() {
    pthread_t politicians[NUM_POLITICIANS];
    int ids[NUM_POLITICIANS];
     int i;
    // Initialize semaphores and mutex
    sem_init(&room, 0, NUM_AREAS); // Allow up to NUM_AREAS politicians to vote simultaneously
    for (i = 0; i < NUM_AREAS; i++) {
        sem_init(&booths[i], 0, 1);
    }
    pthread_mutex_init(&vote_lock, NULL);

    // Create politician threads
    for ( i = 0; i < NUM_POLITICIANS; i++) {
        ids[i] = i;
        pthread_create(&politicians[i], NULL, politician, &ids[i]);
    }

    // Wait for politician threads to finish
    for ( i = 0; i < NUM_POLITICIANS; i++) {
        pthread_join(politicians[i], NULL);
    }

    // Assign unique votes to each politician for demonstration
    for ( i = 0; i < NUM_POLITICIANS; i++) {
        votes[i] = rand() % 100 + 1; // Random votes between 1 and 100
    }

    // Determine the winner
    int max_votes = 0;
    int winner = -1;
    for ( i = 0; i < NUM_POLITICIANS; i++) {
        if (votes[i] > max_votes) {
            max_votes = votes[i];
            winner = i;
        }
    }

    // Print results in tabular form
    printf("\nElection Results:\n");
    printf("+----------------+---------------+\n");
    printf("| Politician ID  | Votes Received|\n");
    printf("+----------------+---------------+\n");
    for ( i = 0; i < NUM_POLITICIANS; i++) {
        printf("|      %d         |       %d       |\n", i, votes[i]);
    }
    printf("+----------------+---------------+\n");
    printf("The winner is Politician %d with %d votes.\n", winner, max_votes);

    // Destroy semaphores and mutex
    for (i = 0; i < NUM_AREAS; i++) {
        sem_destroy(&booths[i]);
    }
    sem_destroy(&room);
    pthread_mutex_destroy(&vote_lock);

    return 0;
}

