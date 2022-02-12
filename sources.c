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

int i, j, m, n, position, tempo;
int m_id, p_id, s_id, q_id;

struct sigaction sa;
struct map *shared_map;
struct param *parameters;
struct message msg_buf;
struct sembuf myops; /*array di operazioni*/
struct timespec gap;
void signalHandler(int sig);

int main(int argc, char **argv){
    int find = 0;
    int b;

    tempo = time(NULL);
    gap.tv_sec = 4;
    gap.tv_nsec = 0;
    
    
    m_id = shmget(SHM_KEY_MAP, sizeof(*shared_map), 0666);
    shared_map = shmat(m_id, NULL, 0);

    p_id = shmget(SHM_KEY_PARAM, sizeof(*parameters), 0600);
    parameters = shmat(p_id, NULL, 0);

    s_id = semget(SEM_KEY, SEM_NUM, 0600);

    q_id = msgget(MSG_QUEUE_KEY, 0600 | IPC_CREAT);
    
    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;

    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }

    b=0;
    while(!b){
        i = my_random(0, SO_HEIGHT-1, tempo + getpid());
        tempo++;
        j = my_random(0, SO_WIDTH-1, tempo + getpid());
        tempo++;
        shm_acquire(s_id);
        if(shared_map->matrix[i][j].isHole != 1 && 
            shared_map->matrix[i][j].isSource!= 1){
            printf("Sources %d caricata in posizione: %d\n", getpid(), shared_map->matrix[i][j].pos);
            shared_map->matrix[i][j].isSource = 1;
            b = 1;
            source_add_sem(s_id);
        }
        shm_release(s_id);
    }
    
    wait_all(s_id);
    
    while(1){
        m = my_random(0, SO_HEIGHT-1, tempo + getpid());
        tempo++;
        n = my_random(0, SO_WIDTH-1, tempo + getpid());
        tempo++;

        if(shared_map->matrix[m][n].isHole != 1 
            && (m!=i || n!=j)){
            msg_buf.mtype = shared_map->matrix[i][j].pos; 
            msg_buf.dest_i = m;
            msg_buf.dest_j = n;
            sprintf(msg_buf.message, "Richiesta effettuata da %d" , getpid());
            queue_acquire(s_id);
            msgsnd(q_id, &msg_buf, sizeof(msg_buf), 0);
            queue_release(s_id);
        }
        
        nanosleep(&gap, NULL);
    }    
        
}

void signalHandler(int sig){
    switch (sig) {   
        case SIGQUIT:
            m = my_random(0, SO_HEIGHT-1, tempo + getpid());
            tempo++;
            n = my_random(0, SO_WIDTH-1, tempo + getpid());
            tempo++;
            while(shared_map->matrix[m][n].isHole == 1 
               || (m==i && n==j)){
                m = my_random(0, SO_HEIGHT-1, tempo + getpid());
                tempo++;
                n = my_random(0, SO_WIDTH-1, tempo + getpid());
                tempo++;   
            }
            msg_buf.mtype = shared_map->matrix[i][j].pos; 
            msg_buf.dest_i = m;
            msg_buf.dest_j = n;
            sprintf(msg_buf.message, "Richiesta effettuata da %d" , getpid());
            queue_acquire(s_id);
            msgsnd(q_id, &msg_buf, sizeof(msg_buf), 0);
            queue_release(s_id);
            printf("Richiesta inserita!\n");
            break;
        case SIGINT:
            msgctl(q_id, IPC_RMID, NULL);
            semctl(s_id, IPC_RMID, 0);
            shmctl(m_id, IPC_RMID, 0);
            shmctl(p_id, IPC_RMID, 0);
            exit(EXIT_SUCCESS);
            break;
        case SIGUSR1:
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}