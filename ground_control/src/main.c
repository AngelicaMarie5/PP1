#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define SHM_NAME "/SharedMemory"
#define PLANES_LIMIT 20
int planes = 0;
int takeoffs = 0;
int traffic = 0;

void Sigusr1_Handler(){

}

void Sigterm_Handler(){

}

void Traffic(int signum) {
  // TODO:
  // Calculate the number of waiting planes.
  int remaining_planes = 
  // Check if there are 10 or more waiting planes to send a signal and increment
  // planes. Ensure signals are sent and planes are incremented only if the
  // total number of planes has not been exceeded.
}

int main(int argc, char* argv[]) {
  // TODO:
  // 1. Open the shared memory block and store this process PID in position 2
  //    of the memory block.
  // 3. Configure SIGTERM and SIGUSR1 handlers
  //    - The SIGTERM handler should: close the shared memory, print
  //      "finalization of operations..." and terminate the program.
  //    - The SIGUSR1 handler should: increase the number of takeoffs by 5.
  // 2. Configure the timer to execute the Traffic function.

  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666 );
  shared_PIDs =
      (int*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_PIDs == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  struct sigaction sa2;
  struct sigaction sa3;
  sa2.sa_handler = Sigusr1_Handler;
  sa3.sa_handler = Sigterm_Handler;
  sigemptyset(&sa2.sa_mask);
  sigemptyset(&sa3.sa_mask);
  sa2.sa_flags = 0;
  sa3.sa_flags = 0;

  if(sigaction(SIGUSR2, &sa2, NULL) == -1){
    perror("sigaction sigusr1");
    exit(EXIT_FAILURE);
  }

  if(sigaction(SIGUSR2, &sa3, NULL) == -1){
    perror("sigaction sigterm");
    exit(EXIT_FAILURE);
  }
  // start timer
  
}