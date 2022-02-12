#include "header.h"
#include "semaphore.h"

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

int m_id, p_id, s_id, q_id, stats_id;
int dest_i, dest_j, start_i, start_j;
int crossed_cells, requests_ok, max_time_travel, travel_started;
struct map *shared_map;
struct param *parameters;
struct stats * stats;
struct message msg_buf;
struct sembuf myops; /*array di operazioni*/
struct timespec gap;
struct sigaction sa;

int i, j, res, random_move;

void signalHandler(int sig);
void move_to_dest();
void move();


void moveUp();
void moveDown();
void moveLeft();
void moveRight();
int checkUp();
int checkDown();
int checkLeft();
int checkRight();

int main(int argc , char ** argv){
    
    
    int taxi_added = 0;
    int tempo = time(NULL);
    

    m_id = shmget(SHM_KEY_MAP, sizeof(*shared_map), 0666);
    shared_map = shmat(m_id, NULL, 0);

    p_id = shmget(SHM_KEY_PARAM, sizeof(*parameters), 0666);
    parameters = shmat(p_id, NULL, 0);

    stats_id = shmget(STATS_KEY, sizeof(*stats), 0600 | IPC_CREAT);
    stats = shmat(stats_id, NULL, 0);

    s_id = semget(SEM_KEY, SEM_NUM, 0600);

    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;
    TEST_ERROR;

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
     
    shm_acquire(s_id);    
    while(!taxi_added){
        i = my_random(0, SO_HEIGHT-1, tempo + getpid());
        tempo++;
        j = my_random(0, SO_WIDTH-1, tempo + getpid());
        tempo++;

        if(shared_map->matrix[i][j].isHole != 1 && 
            shared_map->matrix[i][j].n_taxies < shared_map->matrix[i][j].max_n_taxi){
            shared_map->matrix[i][j].n_taxies++;
            taxi_added = 1;
            taxi_add_sem(s_id);
        }
    }
    shm_release(s_id);
    
    printf("Taxi %d pronto!\n", getpid());
    wait_all(s_id);
    

    /*QUI I TAXI INIZIANO A MUOVERSI*/
    crossed_cells = 0;
    requests_ok = 0;

    while(1){
        max_time_travel = 0;
        res = 0;
        move();
        
        q_id = msgget(MSG_QUEUE_KEY, 0);
        
        res = msgrcv(q_id, &msg_buf, sizeof(msg_buf),shared_map->matrix[i][j].pos, 0600);
        dest_i = msg_buf.dest_i;
        dest_j = msg_buf.dest_j;
        
        travel_started = 1;
        move_to_dest();
        travel_started = 0;

        /*Ogni volta che finisco con successo un viaggio, 
          aggiorno le statistiche e controllo se ci sono state
          variazioni da aggiornare*/
        stats->travel_success += 1;
        requests_ok ++;
        if(requests_ok > stats->taxi_top_request.n){
            stats->taxi_top_request.n = requests_ok;
            stats->taxi_top_request.pid = getpid();
        }
        if(max_time_travel > stats->taxi_top_length_travel.n){
            stats->taxi_top_length_travel.n = max_time_travel;
            stats->taxi_top_length_travel.pid = getpid();
        }
        if(crossed_cells > stats->taxi_top_crossed_cells.n){
            stats->taxi_top_crossed_cells.n = crossed_cells;
            stats->taxi_top_crossed_cells.pid = getpid();
        }
    }

}

/*Fa muovere il taxi in cerca di una source*/
void move(){
    start_i = i;
    start_j = j;
    alarm(parameters->SO_timeout);
    while(shared_map->matrix[start_i][start_j].isSource != 1){
        if(start_i != i || start_j != j){
            j = start_j;
            i = start_i;
            alarm(parameters->SO_timeout);
        }
        shm_acquire(s_id);
        if(checkUp() && shared_map->matrix[start_i-1][start_j].isSource){
            moveUp();
            shm_release(s_id);
            nanosleep(&gap, NULL);
            alarm(0);
        }else if(checkDown() && shared_map->matrix[start_i+1][start_j].isSource){
            moveDown();
            shm_release(s_id);
            nanosleep(&gap, NULL);
            alarm(0);
        }else if(checkLeft() && shared_map->matrix[start_i][start_j-1].isSource){
            moveLeft();
            shm_release(s_id);
            nanosleep(&gap, NULL);
            alarm(0);
        }else if(checkRight() && shared_map->matrix[start_i][start_j+1].isSource){
            moveRight();
            shm_release(s_id);
            nanosleep(&gap, NULL);
            alarm(0);
        }else{

            random_move = 0;
            random_move = my_random(1, 4, time(NULL));
            
            switch(random_move){
                case 1:
                    if(checkUp()){
                        moveUp();
                        shm_release(s_id);
                        nanosleep(&gap, NULL);
                        alarm(0);
                    }else{
                        shm_release(s_id);
                    }
                    break;
                case 2:
                    if(checkDown()){
                        moveDown();
                        shm_release(s_id);
                        nanosleep(&gap, NULL);
                        alarm(0);
                    }else{
                        shm_release(s_id);
                    }
                    break;
                case 3:
                    if(checkLeft()){
                        moveLeft();
                        shm_release(s_id);
                        nanosleep(&gap, NULL);
                        alarm(0);
                    }else{
                        shm_release(s_id);
                    }
                    break;
                case 4:
                    if(checkRight()){
                        moveRight();
                        shm_release(s_id);
                        nanosleep(&gap, NULL);
                        alarm(0);
                    }else{
                        shm_release(s_id);
                    }
                    break;
                default:
                    break;

            }
        }
        
    }
    if(start_i != i || start_j != j){
        j = start_j;
        i = start_i;
        alarm(parameters->SO_timeout);
    }
}

/*Fa muovere il taxi fino alla destinazione*/
void move_to_dest(){
    alarm(parameters->SO_timeout);
    dest_i = msg_buf.dest_i;
    dest_j = msg_buf.dest_j;
    start_i = i;
    start_j = j;
    
    while(start_i != dest_i || start_j != dest_j){
        
        if(start_i != i || start_j != j){
            j = start_j;
            i = start_i;
            alarm(parameters->SO_timeout);
        }
        shm_acquire(s_id);
        if(start_i > dest_i){ /* La nostra i di partenza è maggiore, devo andare su! */
            if(checkUp()){
                moveUp();
                shm_release(s_id);
                nanosleep(&gap, NULL);
                alarm(0);
            }
            else if(start_j < dest_j){
                if(checkRight()){
                    moveRight();
                    shm_release(s_id);
                    nanosleep(&gap, NULL);
                    alarm(0);
                }else{
                    random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkDown()){
                            moveDown();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkLeft()){
                            moveLeft();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
                }
            }
            else if(start_j > dest_j){
                if(checkLeft()){
                    moveLeft();
                    shm_release(s_id);
                    nanosleep(&gap, NULL);
                    alarm(0);
                }else{
                    random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkDown()){
                            moveDown();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkRight()){
                            moveRight();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
                }
            }
            else{
                random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkLeft()){
                            moveLeft();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkRight()){
                            moveRight();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
            }

        }
        else if(start_i < dest_i){ /* La nostra i di partenza è minore, devo andare giù! */
            if(checkDown()){
                moveDown();
                shm_release(s_id);
                nanosleep(&gap, NULL);
                alarm(0);
            }else if(start_j < dest_j){
                if(checkRight()){
                    moveRight();
                    shm_release(s_id);
                    nanosleep(&gap, NULL);
                    alarm(0);
                }else{
                    random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkUp()){
                            moveUp();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkLeft()){
                            moveLeft();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
                }
            }
            else if(start_j > dest_j){
                if(checkLeft()){
                    moveLeft();
                    shm_release(s_id);
                    nanosleep(&gap, NULL);
                    alarm(0);
                }else{
                    random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkUp()){
                            moveUp();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkRight()){
                            moveRight();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
                }
            }
            else{
                random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkLeft()){
                            moveLeft();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkRight()){
                            moveRight();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
            }
        }
        else{
            if(start_j < dest_j){
                if(checkRight()){
                    moveRight();
                    shm_release(s_id);
                    nanosleep(&gap, NULL);
                    alarm(0);
                }
                else{
                    random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkDown()){
                            moveDown();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkUp()){
                            moveUp();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
                }
            }
            else if(start_j > dest_j){
                if(checkLeft()){
                    moveLeft();
                    shm_release(s_id);
                    nanosleep(&gap, NULL);
                    alarm(0);
                }else{
                    random_move = my_random(1,2, time(NULL));
                    if(random_move == 1){
                        if(checkDown()){
                            moveDown();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }else{
                        if(checkUp()){
                            moveUp();
                            shm_release(s_id);
                            nanosleep(&gap, NULL);
                            alarm(0);
                        }else{
                            shm_release(s_id);
                        }
                    }
                }
            }
            else{
                shm_release(s_id);
            }
            
        }
    
    }
    if(start_i == dest_i && start_j == dest_j){
        i = dest_i;
        j = dest_j;
    }
}


/*metodi check per verificare la presenza o meno di buchi sopra, sotto, a dx o a sx del taxi.*/
int checkUp(){
    int cond1 = start_i-1 >= 0;
    int cond2 = shared_map->matrix[start_i-1][start_j].isHole != 1;
    int cond3 = shared_map->matrix[start_i-1][start_j].n_taxies < shared_map->matrix[start_i-1][start_j].max_n_taxi;
    if(cond1 && cond2 && cond3){
        return 1;
    }else{
        return 0;
    }
}
int checkDown(){    
    int cond1 = start_i + 1 < SO_HEIGHT;
    int cond2 = shared_map->matrix[start_i+1][start_j].isHole != 1;
    int cond3 = shared_map->matrix[start_i+1][start_j].n_taxies < shared_map->matrix[start_i+1][start_j].max_n_taxi;
    if(cond1 && cond2 && cond3){
        return 1;
    }else{
        return 0;
    }
}
int checkRight(){
    int cond1 = start_j+1 < SO_WIDTH;
    int cond2 = shared_map->matrix[start_i][start_j+1].isHole != 1;
    int cond3 = shared_map->matrix[start_i][start_j+1].n_taxies < shared_map->matrix[start_i][start_j+1].max_n_taxi;
    if(cond1 && cond2 && cond3){
        return 1;
    }else{
        return 0;
    }
}
int checkLeft(){
    int cond1 = start_j-1 >= 0;
    int cond2 = shared_map->matrix[start_i][start_j-1].isHole != 1;
    int cond3 = shared_map->matrix[start_i][start_j-1].n_taxies < shared_map->matrix[start_i][start_j-1].max_n_taxi;
    if(cond1 && cond2 && cond3){
        return 1;
    }else{
        return 0;
    }
}


void moveUp(){
    /*SPOSTAMENTO*/
    shared_map->matrix[start_i][start_j].n_taxies--;
    shared_map->matrix[start_i-1][start_j].n_taxies++;
    /*STATS*/
    shared_map->matrix[start_i][start_j].crossed ++;
    crossed_cells ++;
    max_time_travel += shared_map->matrix[start_i][start_j].crossing_time;
    /*NANOSLEEP*/
    gap.tv_nsec = shared_map->matrix[start_i][start_j].crossing_time;
    gap.tv_sec = 0;

    start_i -= 1;
}
void moveDown(){
    /*SPOSTAMENTO*/
    shared_map->matrix[start_i][start_j].n_taxies--;
    shared_map->matrix[start_i+1][start_j].n_taxies++;
    /*STATS*/
    shared_map->matrix[start_i][start_j].crossed ++;
    crossed_cells ++;
    max_time_travel += shared_map->matrix[start_i][start_j].crossing_time;
    /*NANOSLEEP*/
    gap.tv_nsec = shared_map->matrix[start_i][start_j].crossing_time;
    gap.tv_sec = 0;
    
    start_i += 1;
}
void moveRight(){
    /*SPOSTAMENTO*/
    shared_map->matrix[start_i][start_j].n_taxies--;
    shared_map->matrix[start_i][start_j+1].n_taxies++;
    /*STATS*/
    shared_map->matrix[start_i][start_j].crossed ++;
    crossed_cells ++;
    max_time_travel += shared_map->matrix[start_i][start_j].crossing_time;
    /*NANOSLEEP*/
    gap.tv_nsec = shared_map->matrix[start_i][start_j].crossing_time;
    gap.tv_sec = 0;

    start_j += 1;
}
void moveLeft(){
    /*SPOSTAMENTO*/
    shared_map->matrix[start_i][start_j].n_taxies--;
    shared_map->matrix[start_i][start_j-1].n_taxies++;
    /*STATS*/
    crossed_cells ++;
    shared_map->matrix[start_i][start_j].crossed ++;
    max_time_travel += shared_map->matrix[start_i][start_j].crossing_time;
    /*NANOSLEEP*/
    gap.tv_nsec = shared_map->matrix[start_i][start_j].crossing_time;
    gap.tv_sec = 0;

    start_j -= 1;
}

void signalHandler(int sig){
    switch (sig) {   
        case SIGALRM :
            if(semctl(s_id, ID_SHM, GETVAL) == 0){
                shm_release(s_id);
            }
            TEST_ERROR;
            stats->travel_aborted ++;
            shared_map->matrix[start_i][start_j].n_taxies--;
            
            if(crossed_cells > stats->taxi_top_crossed_cells.n){
                stats->taxi_top_crossed_cells.n = crossed_cells;
                stats->taxi_top_crossed_cells.pid = getpid();
            }
            
            /*Fase di reborn*/
            kill(getppid(), SIGUSR2);
            TEST_ERROR;
            exit(EXIT_SUCCESS);
            break;
        case SIGINT:
            msgctl(q_id, IPC_RMID, NULL);
            semctl(s_id, IPC_RMID, 0);
            shmctl(m_id, IPC_RMID, 0);
            shmctl(p_id, IPC_RMID, 0);
            shmctl(stats_id, IPC_RMID, 0);
            exit(EXIT_SUCCESS);
            break;
        case SIGUSR1:
            if(travel_started == 1){
                stats->travel_unfinished++;
            }
            if(crossed_cells > stats->taxi_top_crossed_cells.n){
                stats->taxi_top_crossed_cells.n = crossed_cells;
                stats->taxi_top_crossed_cells.pid = getpid();
            }
            if(requests_ok > stats->taxi_top_request.n){
                stats->taxi_top_request.n = requests_ok;
                stats->taxi_top_request.pid = getpid();
            }
            if(max_time_travel > stats->taxi_top_length_travel.n){
                stats->taxi_top_length_travel.n = max_time_travel;
                stats->taxi_top_length_travel.pid = getpid();
            }
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}