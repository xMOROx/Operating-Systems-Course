#ifndef __SIMULATION_CONFIG__
#define __SIMULATION_CONFIG__

#ifdef POSIX
#define IDENTIFICATOR "SHARED"
#define SEMAPHORE_HALL_NAME "/hall_queue_semaphore"
#define SEMAPHORE_CHAIRS_NAME "/chairs_semaphore"
#define SEMAPHORE_BARBERS_NAME "/barbers_semaphore"
#define SEMAPHORE_CLIENTS_NAME "/clients_semaphore"
#endif
#ifdef SYSTEM_V
#define IDENTIFICATOR getenv("HOME")
#define SEMAPHORE_HALL_NAME "0"
#define SEMAPHORE_CHAIRS_NAME "1"
#define SEMAPHORE_BARBERS_NAME "2"
#define SEMAPHORE_CLIENTS_NAME "3"
#endif

#define BARBER_MAX_AMOUNT 5      // M
#define CHAIR_MAX_AMOUNT 3       // N
#define HALL_PLACES_MAX_AMOUNT 8 // P
#define CLIENTS_MAX_AMOUNT 10
#define BUFFOR_SIZE 2048

#endif