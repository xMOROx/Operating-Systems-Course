#ifndef __SIMULATION_CONFIG__
#define __SIMULATION_CONFIG__

#ifdef POSIX
#define IDENTIFICATOR "HAIRDRESSER"
#define SEMAPHORE_HALL_NAME "/hall_queue_semaphore"
#define SEMAPHORE_CHAIRS_NAME "/chairs_semaphore"
#define SEMAPHORE_BARBERS_NAME "/barbers_semaphore"
#define SEMAPHORE_CLIENTS_NAME "/clients_semaphore"
#endif
#ifdef SYSV
#define IDENTIFICATOR getenv("HOME")
#define SEMAPHORE_HALL_NAME "0"
#define SEMAPHORE_CHAIRS_NAME "1"
#define SEMAPHORE_BARBERS_NAME "2"
#define SEMAPHORE_CLIENTS_NAME "3"
#endif

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define YELLOW "\033[0;33m"
#define CYAN "\033[0;36m"
#define MAGENTA "\033[0;35m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

#define BARBER_MAX_AMOUNT 5      // M
#define CHAIR_MAX_AMOUNT 3       // N
#define HALL_PLACES_MAX_AMOUNT 8 // P
#define CLIENTS_AMOUNT 10
#define BUFFOR_SIZE 2048

#endif