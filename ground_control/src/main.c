#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

// #include "../air_control_c/include/functions.h"
// Traté de usar este include para no tener que declarar las variables de nuevo,
// y funcionó, pero falla las compilación del test

#define SHM_NAME "/SharedMemory"
#define SHM_SIZE sizeof(int) * 3
int* shared_PIDs;

#define PLANES_LIMIT 20
int planes = 0;
int takeoffs = 0;
int traffic = 0;

int shm_fd;

pthread_mutex_t state_lock;
pthread_mutex_t runway1_lock;
pthread_mutex_t runway2_lock;

void SIGUSR1_handler(int signal) {
  pthread_mutex_lock(&state_lock);
  takeoffs += 5;
  pthread_mutex_unlock(&state_lock);
}

void SIGTERM_handler(int signal) {
  // Clean Up
  printf("Fin de operaciones...");
  close(shm_fd);
}

void Traffic(int signum) {
  // TODO:
  // Calculate the number of waiting planes.
  int num_planes_waiting;
  pthread_mutex_lock(&state_lock);
  num_planes_waiting = planes - takeoffs;
  pthread_mutex_unlock(&state_lock);

  // Check if there are 10 or more waiting planes to send a signal and increment
  // planes. Ensure signals are sent and planes are incremented only if the
  // total number of planes has not been exceeded.
  pthread_mutex_lock(&state_lock);
  if (num_planes_waiting >= 10) {
    printf("RUNWAY OVERLOADED");
  }
  if (num_planes_waiting < PLANES_LIMIT) {
    planes += 5;
    kill(shared_PIDs[1], SIGUSR2);
  }
  pthread_mutex_unlock(&state_lock);
}

int main(int argc, char* argv[]) {
  pthread_mutex_init(&state_lock, NULL);
  pthread_mutex_init(&runway1_lock, NULL);
  pthread_mutex_init(&runway2_lock, NULL);

  // TODO:
  // 1. Open the shared memory block and store this process PID in position 2
  //    of the memory block.
  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);

  shared_PIDs =
      (int*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_PIDs == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  shared_PIDs[2] = getpid();
  // 3. Configure SIGTERM and SIGUSR1 handlers
  //    - The SIGTERM handler should: close the shared memory, print
  //      "finalization of operations..." and terminate the program.
  //    - The SIGUSR1 handler should: increase the number of takeoffs by 5.

  struct sigaction sa = {0};
  sa.sa_handler = &SIGUSR1_handler;
  sigaction(SIGUSR1, &sa, NULL);

  struct sigaction sa2 = {0};
  sa.sa_handler = &SIGTERM_handler;
  sigaction(SIGTERM, &sa, NULL);

  // 2. Configure the timer to execute the Traffic function.
  signal(SIGALRM, Traffic);

  struct itimerval timer;
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 500000;  // 500 ms
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 500000;
  setitimer(ITIMER_REAL, &timer, NULL);

  return 0;
}