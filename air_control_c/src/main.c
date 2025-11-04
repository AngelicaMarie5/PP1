#include "functions.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


int main() {
  // TODO 1: Call the function that creates the shared memory segment.
  MemoryCreate();

  // TODO 3: Configure the SIGUSR2 signal to increment the planes on the runway
  // by 5.
  Configure_SIGUSR2();
  // TODO 4: Launch the 'radio' executable and, once launched, store its PID in
  // the second position of the shared memory block.
  int pid_1 = fork();
  if(pid_1 < 0){
    perror("pid_1 fork error");
    exit(EXIT_FAILURE);
  }

  if(pid_1 == 0) {
    int child_pid = getpid();
    // fprintf("child proccess, pid %i", child_pid);

    int child_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(child_fd == -1){
      perror("child fd error");
      exit(EXIT_FAILURE);
    }

    int *child_ptr = mmap(NULL, SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, child_fd, 0);
    if(child_ptr == MAP_FAILED){
      perror("child mmap error");
      exit(EXIT_FAILURE);
    }

    child_ptr[1]=getpid();
    munmap(child_ptr, SHM_SIZE);
    execl("../radio", "radio", SHM_NAME, (char *)NULL);
    _exit(EXIT_FAILURE);
  }
  // TODO 6: Launch 5 threads which will be the controllers; each thread will
  // execute the TakeOffsFunction().

  

 Init_Mutex();

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

  Cleanup_SHM();
  Destroy_Mutex();
 
  return 0;
}
