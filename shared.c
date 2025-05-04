#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/wait.h>
#include<string.h>
#include<semaphore.h>

#define SHARED_MEMORY_SIZE 8192 
sem_t semaphore;

int main() {
    sem_init(&semaphore, 1, 1);
    key_t key = ftok("customer.txt", 'A');

    int shmid = shmget(key, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    }

    char *shared_memory = (char*)shmat(shmid, NULL, 0);
    if (shared_memory == (char*)-1) {
        perror("Error attaching shared memory");
        exit(EXIT_FAILURE);
    }

    pid_t ChildProcess = fork();

    if (ChildProcess == -1) {
        perror("Fork creation failed");
        exit(EXIT_FAILURE);
    } else if (ChildProcess == 0) {
        sem_wait(&semaphore);
        printf("Child Process Created\n");
        FILE *file2 = fopen("destination.txt", "w");
        if (!file2) {
            perror("Error opening destination file");
            exit(EXIT_FAILURE);
        }
        fprintf(file2, "%s", shared_memory);
        fclose(file2);
        printf("Child process done with writing\n");
        sem_post(&semaphore);
        exit(0);
    } else {
        sem_wait(&semaphore);
        printf("Parent Process Created\n");
        FILE *file1 = fopen("source.txt", "r");
        if (!file1) {
            perror("Error opening source file");
            exit(EXIT_FAILURE);
        }

        char buffer[512]; 
        size_t offset = 0;

        while (fgets(buffer, sizeof(buffer), file1) != NULL) {
            size_t len = strlen(buffer);
            if (offset + len < SHARED_MEMORY_SIZE) {
                memcpy(shared_memory + offset, buffer, len);
                offset += len;
            } else {
                fprintf(stderr, "Shared memory limit exceeded\n");
                break;
            }
        }
        fclose(file1);

        sem_post(&semaphore);

        wait(NULL);

        if (shmdt(shared_memory) == -1) {
            perror("Error detaching shared memory");
        }

        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("Error removing shared memory");
        }

        sem_destroy(&semaphore);
    }

    return 0;
}