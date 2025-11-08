#ifndef INCLUDE_FUNCTIONS_H_
#define INCLUDE_FUNCTIONS_H_

extern int* shared_PIDs;

#define SHM_NAME "/SharedMemory"
#define SHM_SIZE sizeof(int) * 3

void MemoryCreate();
void* TakeOffsFunction();
void SIGUSR2_handler(int signal);

#endif  // INCLUDE_FUNCTIONS_H_