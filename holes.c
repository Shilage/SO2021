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


int m_id, p_id, s_id;
struct sigaction sa;
struct sembuf myops;
void signalHandler(int sig);


int main (int argc , char ** argv){
    struct map *shared_map;
    struct param *parameters;
    int i, j, holes_request;
    int tempo = time(NULL);

    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;

    if(sigaction(SIGALRM, &sa, NULL) == -1){
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if(sigaction(SIGINT, &sa, NULL) == -1){
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    
    s_id = semget(SEM_KEY, SEM_NUM, 0600 | IPC_CREAT);
    semctl(s_id, ID_SHM, SETVAL, 1);

    m_id = shmget(SHM_KEY_MAP, sizeof(*shared_map), 0666);
    shared_map = shmat(m_id, NULL, 0);

    p_id = shmget(SHM_KEY_PARAM, sizeof(*parameters), 0666);
    parameters = shmat(p_id, NULL, 0);
    
    myops.sem_flg = 0;
    
    holes_request = parameters->SO_holes;

    alarm(1);
    while(parameters->SO_holes){
        i = my_random(0, SO_HEIGHT-1, tempo);
        tempo++;
        j = my_random(0, SO_WIDTH-1, tempo);
        tempo++;
        if(shared_map->matrix[i][j].isHole == 0){
            if(i == 0 && j == 0){/*angolo alto sx*/
                if(shared_map->matrix[i+1][j].isHole == 0 &&
                shared_map->matrix[i][j+1].isHole == 0 &&
                shared_map->matrix[i+1][j+1].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;      
                }
             }else if(i == SO_HEIGHT-1 && j == 0){/*angolo alto dx*/
                if(shared_map->matrix[i-1][j].isHole == 0 &&
                shared_map->matrix[i-1][j+1].isHole == 0 &&
                shared_map->matrix[i][j+1].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;  
                }
             }else if(i == 0 && j == SO_WIDTH-1){/*angolo basso sx*/
                if(shared_map->matrix[i][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;         
                }
             }else if(i == SO_HEIGHT-1 && j == SO_WIDTH-1){/*angolo basso dx*/
                if(shared_map->matrix[i][j-1].isHole == 0 &&
                shared_map->matrix[i-1][j-1].isHole == 0 &&
                shared_map->matrix[i-1][j].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;          
                }
             }else if(i == 0){/*lato sx*/
                if(shared_map->matrix[i][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j].isHole == 0 &&
                shared_map->matrix[i+1][j+1].isHole == 0 &&
                shared_map->matrix[i][j+1].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;         
                }
             }else if(j == 0){/*lato up*/
                if(shared_map->matrix[i-1][j].isHole == 0 &&
                shared_map->matrix[i-1][j+1].isHole == 0 &&
                shared_map->matrix[i][j+1].isHole == 0 &&
                shared_map->matrix[i+1][j+1].isHole == 0 &&
                shared_map->matrix[i+1][j].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;           
                }
             }else if(i == SO_HEIGHT-1){/*lato dx*/
                if(shared_map->matrix[i][j-1].isHole == 0 &&
                shared_map->matrix[i-1][j-1].isHole == 0 &&
                shared_map->matrix[i-1][j].isHole == 0 &&
                shared_map->matrix[i-1][j+1].isHole == 0 &&
                shared_map->matrix[i][j+1].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;        
                }
             }else if(j == SO_WIDTH-1){/*lato dw*/
                if(shared_map->matrix[i-1][j].isHole == 0 &&
                shared_map->matrix[i-1][j-1].isHole == 0 &&
                shared_map->matrix[i][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;          
                }
             }else{
                if(shared_map->matrix[i][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j-1].isHole == 0 &&
                shared_map->matrix[i+1][j].isHole == 0 &&
                shared_map->matrix[i+1][j+1].isHole == 0 &&
                shared_map->matrix[i][j+1].isHole == 0 &&
                shared_map->matrix[i-1][j+1].isHole == 0 &&
                shared_map->matrix[i-1][j].isHole == 0 &&
                shared_map->matrix[i-1][j-1].isHole == 0){

                    parameters->SO_max_taxi -= shared_map->matrix[i][j].max_n_taxi;
                    shared_map->matrix[i][j].isHole = 1;
                    parameters->SO_holes--;         
                }
            }
        }
        shared_map->holes_in = holes_request - parameters->SO_holes;
    }
    return 0;
}

void signalHandler(int sig){
    switch (sig){
    case SIGALRM:
        kill(getppid(), SIGTERM);
        exit(EXIT_SUCCESS);
        break;
    case SIGINT:
        semctl(s_id, IPC_RMID, 0);
        shmctl(m_id, IPC_RMID, 0);
        shmctl(p_id, IPC_RMID, 0);
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
    
}