#include "semaphore.h"
#include "header.h"

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

struct sembuf my_ops;
int sem_id;

void checkError(){
    if(semop(sem_id, &my_ops, 1)==-1){
        switch (errno) {
            case EINVAL:
                printf("%d\n", sem_id);
                printf("PID = %d, riga:%d : semaforo rimosso prima di blocco (o mai esistito)\n", getpid(), __LINE__);
                exit(EXIT_FAILURE);
            case EAGAIN:
                printf("PID = %d, riga:%d : mi sono stufato di aspettare\n", getpid(), __LINE__);
                exit(EXIT_FAILURE);
            case EINTR:
                break ;
            default:
                break ;
        }
    }
}

void shm_release(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num =ID_SHM;
    my_ops.sem_op = 1;  
    checkError(); 
}

void shm_acquire(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num = ID_SHM;
    my_ops.sem_op = -1;
    checkError();
}

void queue_acquire(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num = ID_MSG_QUEUE;
    my_ops.sem_op = -1;
    
    checkError();
}

void queue_release(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num = ID_MSG_QUEUE;
    my_ops.sem_op = 1;
    checkError();
}

void taxi_add_sem(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num  = ID_TAXI;
    my_ops.sem_op = 1;
    checkError();
}

void source_add_sem(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num  = ID_SOURCES;
    my_ops.sem_op = 1;
    checkError();
}


void wait_all(int id){
    sem_id = id;
    my_ops.sem_flg = 0;
    my_ops.sem_num  = ID_WAIT_FOR;
    my_ops.sem_op = 0;
    checkError();
}
