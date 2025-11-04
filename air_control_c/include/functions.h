#ifndef INCLUDE_FUNCTIONS_H_
#define INCLUDE_FUNCTIONS_H_

#define TOTAL_TAKEOFFS 50
#define SHM_NAME "/SharedMemory"
#define SHM_SIZE sizeof(int) * 3

extern int* shared_PIDs;

void* TakeOffsFunction();
void MemoryCreate(void);
void Sig2_Handler(void);
void Configure_SIGUSR2(void);
void Destroy_Mutex();
void Init_Mutex();
void Cleanup_SHM();

#endif  // INCLUDE_FUNCTIONS_H_