#ifndef HEADER
#define HEADER
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#ifdef DENSE
#define SO_HEIGHT 10
#define SO_WIDTH 20
#elif LARGE
#define SO_HEIGHT 20
#define SO_WIDTH 60
#elif CUSTOM
#define SO_HEIGHT 10
#define SO_WIDTH 10
#endif

#define SHM_KEY_MAP 52555
#define SHM_KEY_PARAM 32455
#define STATS_KEY 12444
#define MSG_QUEUE_KEY 14155

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


#define TEST_ERROR    if (errno) {dprintf(STDERR_FILENO,		\
					  "%s:%d: PID=%5d: Error %d (%s)\n", \
					  __FILE__,			\
					  __LINE__,			\
					  getpid(),			\
					  errno,			\
					  strerror(errno));}

/*cella della matrice in memoria condivisa*/
typedef struct cell{
    int crossing_time;  /*tempo di attraversamento di cella*/
    int max_n_taxi;     /*capacità massima cella*/
    int n_taxies;       /*numero taxi presenti in cella*/
    int crossed;        /*numero attraversamenti cella*/
    int isHole;         /*vale 1 se la cella è inaccessibile, 0 altrimenti*/
    int isSource;       /*vale 1 se la cella è una richiesta, 0 altrimenti*/
    int pos;            /*indica la posizione nella matrice */
} cell;

/*mappa di gioco*/
struct map{
    cell matrix [SO_HEIGHT][SO_WIDTH];
    int holes_in; /*buchi effettivamente inseriti*/
};

/*Parametri per la simulazione di gioco*/
struct param{
    int SO_holes;
    int SO_taxies;
    int SO_sources;
    int SO_top_cells;
    int SO_timesec_min;
    int SO_timesec_max;
    int SO_capacity_min;
    int SO_capacity_max;
    int SO_max_taxi;
    int SO_timeout;
    int SO_duration;
};

/*Struttura messaggio per coda di messaggi*/
struct message{
    long mtype;
    char message[30];
    int dest_i;
    int dest_j;
};

/*record per salvare statistiche*/
typedef struct record{
    int pid;
    int n;
}record;

/*statistiche*/
struct stats{
    int travel_success;
    int travel_aborted;
    int travel_unfinished;
    int in_search;
    record taxi_top_crossed_cells;
    record taxi_top_length_travel;
    record taxi_top_request;
};


int my_random(int min , int max, int seed); /*generate random number between min and max*/
void print_map(struct map* map); 
void print_final_map(struct map* map);
void get_parameters(struct param* p);
cell * insert_top_cells (struct map * shared_map); 
void sort(cell * a);
void print_top_cells(int dim, cell * top);

#endif