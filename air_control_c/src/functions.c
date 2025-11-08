#include "functions.h"

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define TOTAL_TAKEOFFS 20

int planes = 0;
int takeoffs = 0;
int total_takeoffs = 0;

int* shared_PIDs;

pthread_mutex_t state_lock;
pthread_mutex_t runway1_lock;
pthread_mutex_t runway2_lock;

void SIGUSR2_handler(int signal) {
  pthread_mutex_lock(&state_lock);
  planes += 5;
  pthread_mutex_unlock(&state_lock);
}

void MemoryCreate() {
  // TODO2: create the shared memory segment, configure it and store the PID of
  // the process in the first position of the memory block.

  // Create shared memory object
  pthread_mutex_init(&state_lock, NULL);
  pthread_mutex_init(&runway1_lock, NULL);
  pthread_mutex_init(&runway2_lock, NULL);

  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    exit(1);
  }

  if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    perror("ftruncate failed");
    exit(1);
  }

  shared_PIDs =
      (int*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_PIDs == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  shared_PIDs[0] = getpid();
}

// void SigHandler2(int signal) {}

void* TakeOffsFunction(void* arg) {
  // TODO: implement the logic to control a takeoff thread
  //    Use a loop that runs while total_takeoffs < TOTAL_TAKEOFFS
  //    Use runway1_lock or runway2_lock to simulate a locked runway
  //    Use state_lock for safe access to shared variables (planes,
  //    takeoffs, total_takeoffs)
  //    Simulate the time a takeoff takes with sleep(1)
  //    Send SIGUSR1 every 5 local takeoffs
  //    Send SIGTERM when the total takeoffs target is reached

  printf("Thread started: ID %lu\n", pthread_self());
  fflush(stdout);

  while (total_takeoffs < TOTAL_TAKEOFFS) {
    int trylock1 = pthread_mutex_trylock(&runway1_lock);
    int trylock2 = pthread_mutex_trylock(&runway2_lock);
    if (trylock1 == 0) {
      printf("Thread %lu acquired runway 1\n", pthread_self());
      fflush(stdout);
      pthread_mutex_lock(&runway1_lock);
      pthread_mutex_lock(&state_lock);
      planes -= 1;
      takeoffs += 1;
      total_takeoffs += 1;

      sleep(1);

      if (takeoffs == 5) {
        takeoffs = 0;
        kill(shared_PIDs[1], SIGUSR1);
      }

      pthread_mutex_unlock(&state_lock);
      pthread_mutex_unlock(&runway1_lock);

    } else if (trylock2 == 0) {
      printf("Thread %lu acquired runway 2\n", pthread_self());
      fflush(stdout);
      pthread_mutex_lock(&runway2_lock);
      pthread_mutex_lock(&state_lock);
      planes -= 1;
      takeoffs += 1;
      total_takeoffs += 1;
      sleep(1);

      if (takeoffs == 5) {
        takeoffs = 0;

        kill(shared_PIDs[1], SIGUSR1);
      }

      pthread_mutex_unlock(&state_lock);
      pthread_mutex_unlock(&runway2_lock);
    }
    sleep(1);
  }

  kill(shared_PIDs[1], SIGTERM);
}
