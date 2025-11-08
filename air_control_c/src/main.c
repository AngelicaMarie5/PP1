#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/functions.h"

#define NUM_THREADS 5

int main() {
  // TODO 1: Call the function that creates the shared memory segment.
  MemoryCreate();

  // TODO 3: Configure the SIGUSR2 signal to increment the planes on the
  // runway by 5.
  struct sigaction sa = {0};
  sa.sa_handler = &SIGUSR2_handler;
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    perror("sigaction sigusr2");
    exit(EXIT_FAILURE);
  }
  // TODO 4: Launch the 'radio' executable and, once launched, store its PID
  // in the second position of the shared memory block.
  int AirControlChild = fork();
  if (AirControlChild == 0) {
    // Child Process
    int child_pid = getpid();
    shared_PIDs[1] = child_pid;
    execl("./radio", "radio", SHM_NAME, NULL);
    perror("execl failed");
    exit(1);

  } else if (AirControlChild > 0) {
    // Parent Process
    sleep(1);

    // TODO 6: Launch 5 threads which will be the controllers; each thread will
    // execute the TakeOffsFunction().
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
      printf("Creating thread #%d\n", i);
      fflush(stdout);
      pthread_create(&threads[i], NULL, TakeOffsFunction, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
      printf("Joining thread #%d\n", i);
      fflush(stdout);
      pthread_join(threads[i], NULL);
    }

    wait(NULL);

  } else {
    perror("air_control fork failed");
    exit(1);
  }

  return 0;
}