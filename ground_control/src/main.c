#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
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

void Traffic(int signum) {
  // TODO:
  // Calculate the number of waiting planes.
  // Check if there are 10 or more waiting planes to send a signal and increment
  // planes. Ensure signals are sent and planes are incremented only if the
  // total number of planes has not been exceeded.
}

int main(int argc, char* argv[]) {
  // TODO:
  // 1. Open the shared memory block and store this process PID in position 2
  //    of the memory block.
  int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);

  shared_PIDs =
      (int*)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_PIDs == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  shared_PIDs[1] = getpid();
  // 3. Configure SIGTERM and SIGUSR1 handlers
  //    - The SIGTERM handler should: close the shared memory, print
  //      "finalization of operations..." and terminate the program.
  //    - The SIGUSR1 handler should: increase the number of takeoffs by 5.
  // 2. Configure the timer to execute the Traffic function.
}