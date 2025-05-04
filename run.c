#include <stdio.h>
#include <stdlib.h>

void runPrograms() {
    // Run the hashing program
    printf("Running hashing program...\n");
    int status = system("./hashing");
    if (status == -1) {
        perror("Error running hashing program");
        exit(EXIT_FAILURE);
    }

    // Run the shared program
    printf("Running shared memory program...\n");
    status = system("./shared");
    if (status == -1) {
        perror("Error running shared memory program");
        exit(EXIT_FAILURE);
    }
}

int main() {
    runPrograms(); // Function to run both programs sequentially
    printf("Done with the code\n");
    return 0;
}