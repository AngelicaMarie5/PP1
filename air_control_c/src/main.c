#include "functions.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define SHM_NAME "/SharedMemory"
#define SHM_SIZE sizeof(int) * 3

int* shm_PIDs;

int planes = 0;
int takeoffs = 0;
int total_takeoffs = 0;

pthread_mutex_t state_lock;
pthread_mutex_t runway1_lock;
pthread_mutex_t runway2_lock;


void Sig2_Handler(){
  planes +=5;

}

int main() {
  // TODO 1: Call the function that creates the shared memory segment.

  // TODO 3: Configure the SIGUSR2 signal to increment the planes on the runway
  // by 5.

  // TODO 4: Launch the 'radio' executable and, once launched, store its PID in
  // the second position of the shared memory block.

  // TODO 6: Launch 5 threads which will be the controllers; each thread will
  // execute the TakeOffsFunction().

  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
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

  shm_PIDs =
      (int*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_PIDs == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  shm_PIDs[0]= getpid();    //air control pid
  shm_PIDs[1]= 0;
  shm_PIDs[2]=0; 

  

  struct sigaction sa;
  sa.sa_handler = Sig2_Handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGUSR2, &sa, NULL);

  int pid_1 = fork();
  if(pid_1 == 0){
    fprintf("child proccess, pid %i", getpid());
    shm_PIDs[1]=getpid();
    execpl("/../../radio");
  }

  pthread_mutex_init(&state_lock, NULL);
  pthread_mutex_init(&runway1_lock, NULL);
  pthread_mutex_init(&runway2_lock, NULL);

  pthread_t thread1, thread2, thread3, thread4, thread5;
  pthread_create(&thread1, NULL, TakeOffsFunction, NULL);
  pthread_create(&thread2, NULL, TakeOffsFunction, NULL);
  pthread_create(&thread3, NULL, TakeOffsFunction, NULL);
  pthread_create(&thread4, NULL, TakeOffsFunction, NULL);
  pthread_create(&thread5, NULL, TakeOffsFunction, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);
  pthread_join(thread4, NULL);
  pthread_join(thread5, NULL);














  pthread_mutex_destroy(&state_lock);
  pthread_mutex_destroy(&runway1_lock);
  pthread_mutex_destroy(&runway2_lock);



  close(shm_fd);
  shm_unlink(SHM_NAME);
  return 0;
}
