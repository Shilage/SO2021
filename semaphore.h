#ifndef SEMAPHORE
#define SEMAPHORE

/*
* Progetto SO 2020-2021 creato da:
*
* Cognome: Tarquini
* Nome: Alice
* Matricola: 861373
* Corso: B
* Turno: 3
*
* Cognome: Luciani
* Nome: Fabio
* Matricola: 863367
* Corso: B
* Turno: 3
*
* Cognome: Marino
* Nome: Giuseppe
* Matricola: 814025
* Corso: B
* Turno: 3
*
*/

#define _GNU_SOURCE

#define SEM_KEY 91243
#define SEM_NUM 5

#define ID_SHM 0
#define ID_TAXI 1
#define ID_WAIT_FOR 2
#define ID_SOURCES 3
#define ID_MSG_QUEUE 4

void shm_acquire(int id);
void shm_release(int id);
void queue_acquire(int id);
void queue_release(int id);
void taxi_add_sem(int id);
void wait_all(int id);
void source_add_sem(int id);


#endif


