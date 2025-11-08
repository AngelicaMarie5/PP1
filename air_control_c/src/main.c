#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/functions.h"

int main() {
  // TODO 1: Call the function that creates the shared memory segment.
  MemoryCreate();

  int AirControlChild = fork();

  if (AirControlChild == 0) {
    // Child Process
    // Instruction says: In the child process, save its PID in the second
    // position of the memory block and run the radio program.
    int child_pid = getpid();
    shared_PIDs[1] = child_pid;
    // Not sure if this is the right absolute path
    execl("../../test/radio", "radio", "/SharedMemory", NULL);
    perror("execl failed");
    exit(1);

  } else if (AirControlChild > 0) {
    // Parent Process
    wait(NULL);

  } else {
    perror("air_control fork failed");
  }

  // TODO 3: Configure the SIGUSR2 signal to increment the planes on the runway
  // by 5.
  struct sigaction sa = {0};
  sa.sa_handler = &SIGUSR2_handler;
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    perror("sigaction sigusr2");
    exit(EXIT_FAILURE);
  }

  // TODO 4: Launch the 'radio' executable and, once launched, store its PID in
  // the second position of the shared memory block.

  // TODO 6: Launch 5 threads which will be the controllers; each thread will
  // execute the TakeOffsFunction().
}