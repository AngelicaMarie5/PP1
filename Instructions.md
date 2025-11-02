# Part 1: C Implementation

## air_control_c
In this first part, you must implement the air_control program, which simulates a basic aircraft takeoff control system using shared memory, signals, and threads in C. The objective is to replicate the interprocess coordination and thread synchronization logic, which will later be extended to the Python implementation. The main program will be responsible for creating shared memory, handling signals indicating the arrival of new aircraft, creating takeoff controller threads, and launching the radio process.

### General Instructions
1. Create shared memory: The program must create a shared memory block large enough to store three integer values. The PIDs of the processes involved in the simulation will be stored in this memory.
2. Configure the signal handler: Register a handler for SIGUSR2 that increments the number of available planes for takeoff by 5.
3. Launch the radio process: The air_control program must create a child process using fork(), and in that process, execute the radio binary using execl(). When launched, it must send the name of the shared memory (SH_MEMORY_NAME) as a parameter.
4. Create the controller threads: Five threads must be created, each executing the TakeOffsFunction function, which will be responsible for managing aircraft takeoffs.
5. Synchronize access to shared variables: Access to global variables and takeoff runways must be controlled using mutexes to avoid race conditions between threads.
6. Once the total number of takeoffs (TOTAL_TAKEOFFS) is reached, the air_control process must send the SIGTERM signal to the radio process and then release the shared memory.

### Shared global variables (all are integers of type int):
- *planes*: number of planes waiting to take off
- *takeoffs*: count of takeoffs performed to account for block of 5 takeoffs and send the SIGUSR1 signal
- *total_takeoffs*: total takeoffs performed

### Mutexes to be used:
- *state_lock*: to protect and synchronize access to the shared variables indicated above, avoiding race conditions.
- *runway1_lock* and *runway2_lock*: mutexes that simulate exclusive use of runways; only one thread can use a runway at a time.

### Specifying tasks to be performed

1. Configure shared memory:
    - Create the memory.
    - Map the memory block.
    - Save the PID of the air_control process in the first position of the memory block.
2. Signal Configuration:
    - Configure the SigHandler2 handler for the SIGUSR2 signal.
    - This handler must increment the planes variable by 5 units each time the signal is received.
3. Launching the radio process:
    - Using fork(), create a child process.
    - In the child process, save its PID in the second position of the memory block and run the radio program.
4. Creating Threads:
    - Create 5 threads that execute the TakeOffsFunction function.
    - Each thread will simulate an independent track controller.
5. Implementing TakeOffsFunction:
    - While total_takeoffs < TOTAL_TAKEOFFS:
    - Attempt to take one of the two tracks using pthread_mutex_trylock().
     - If unsuccessful, sleep with usleep(1000) and retry.
    - Once a runway is obtained, enter the mutex-protected critical 
    section (this is if planes are available):
     - Decrement planes by 1.
     - Increase takeoffs and total_takeoffs by 1.
     - If takeoffs equals 5, send SIGUSR1 signal to the radio process and reset takeoffs = 0.
    - Release the main mutex state_lock.
    - Sleep sleep(1) to simulate the takeoff time.
    - Release the runway (pthread_mutex_unlock() of runway1_lock or runway2_lock as appropriate).
    - Upon completion, when total_takeoffs >= TOTAL_TAKEOFFS, send SIGTERM to the radio process, close shared memory, and return.
6. Release resources: Once all threads have finished, call shm_unlink(SH_MEMORY_NAME) to remove the shared memory.


 


