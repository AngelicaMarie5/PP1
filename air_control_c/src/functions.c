#define _POSIX_C_SOURCE 200809L
#include "functions.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


int planes = 0;
int takeoffs = 0;
int total_takeoffs = 0;

int global_fd = 0;

int* shared_PIDs;

pthread_mutex_t state_lock;
pthread_mutex_t runway1_lock;
pthread_mutex_t runway2_lock;

void MemoryCreate() {
  int shm_fd = shm_open("/SharedMemory", O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    exit(1);
  }

  // Configure size of shared memory object large enough to store three integer
  // values
  if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    perror("ftruncate failed");
    exit(1);
  }
  global_fd = shm_fd;

  shared_PIDs =
      (int*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_PIDs == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  shared_PIDs[0] = getppid();
  shared_PIDs[1] = 0;
  shared_PIDs[2] = 0;
}

void Sig2_Handler(){
  return planes +=5;
  
}

void Configure_SIGUSR2(){
  struct sigaction sa;
  sa.sa_handler = Sig2_Handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if(sigaction(SIGUSR2, &sa, NULL) == -1){
    perror("sigaction sigusr2");
    exit(EXIT_FAILURE);
  }

}

void Destroy_Mutex(){
  pthread_mutex_destroy(&state_lock);
  pthread_mutex_destroy(&runway1_lock);
  pthread_mutex_destroy(&runway2_lock);
}

void Init_Mutex(){
  pthread_mutex_init(&state_lock, NULL);
  pthread_mutex_init(&runway1_lock, NULL);
  pthread_mutex_init(&runway2_lock, NULL);
}

void Cleanup_SHM(){
  close(global_fd);
  shm_unlink(SHM_NAME);
}

void* TakeOffsFunction() {
  int local_takeoffs =0;
  // TODO: implement the logic to control a takeoff thread
  //    Use a loop that runs while total_takeoffs < TOTAL_TAKEOFFS
  //    Use runway1_lock or runway2_lock to simulate a locked runway
  //    Use state_lock for safe access to shared variables (planes,
  //    takeoffs, total_takeoffs)
  //    Simulate the time a takeoff takes with sleep(1)
  //    Send SIGUSR1 every 5 local takeoffs
  //    Send SIGTERM when the total takeoffs target is reached
  while (total_takeoffs < TOTAL_TAKEOFFS){
   int trylock1 = pthread_mutex_trylock(&runway1_lock);
   int trylock2 = pthread_mutex_trylock(&runway2_lock);

   if(trylock1 == 0){
    pthread_mutex_lock(&runway1_lock);
    pthread_mutex_lock(&state_lock);

    planes -= 1;
    takeoffs += 1;
    total_takeoffs += 1;
    sleep(1);

    if(takeoffs == 5){
      takeoffs = 0;
      kill(shared_PIDs[1], SIGUSR1);
    }

    pthread_mutex_unlock(&state_lock);
    pthread_mutex_unlock(&runway1_lock);

   }
   else if (trylock2 == 0){
    pthread_mutex_lock(&runway2_lock);
    pthread_mutex_lock(&state_lock);

    planes -= 1;
    takeoffs += 1;
    total_takeoffs += 1;
    sleep(1);

    if(takeoffs == 5){
      takeoffs = 0;
      kill(shared_PIDs[1], SIGUSR1);
    }

    pthread_mutex_unlock(&state_lock);
    pthread_mutex_unlock(&runway2_lock);
    
   }
   
   sleep(1000);
  }

  kill(shared_PIDs[1], SIGTERM);
}
