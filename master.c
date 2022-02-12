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

struct sigaction sa;
struct map *shared_map;
struct param *parameters;
struct sembuf myops;
struct stats *stats;
cell* top_cells;

int m_id, p_id, s_id, q_id, stats_id, child_pid;
char s_holes[10], s_taxi[10], s_sources[10];
int i,j, timer;
char ** argv;
void signalHandler(int sig);

int main(int argc, char ** argv){
    int crossing_time_random, max_n_taxi_random, position;
    int tempo = time(NULL);
    timer = 0;
    
    
    s_id = semget(SEM_KEY, SEM_NUM, 0600 | IPC_CREAT);
    semctl(s_id, ID_SHM, SETVAL, 1);
    semctl(s_id, ID_TAXI, SETVAL, 0);
    semctl(s_id, ID_WAIT_FOR, SETVAL, 1);
    semctl(s_id, ID_SOURCES, SETVAL, 0);
    semctl(s_id, ID_MSG_QUEUE, SETVAL, 1);
    myops.sem_flg = 0;

    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;
    TEST_ERROR;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        printf("\nErrore in handler disposition\n");
        exit(EXIT_FAILURE);
    }


    /*creare mappa e caricarla in memoria condivisa in modo che la vedano tutti*/
    m_id = shmget(SHM_KEY_MAP, sizeof(*shared_map), 0666| IPC_CREAT);
    shared_map = shmat(m_id, NULL, 0);
    
    p_id = shmget(SHM_KEY_PARAM, sizeof(*parameters), 0600 | IPC_CREAT);
    parameters = shmat(p_id, NULL, 0);

    stats_id = shmget(STATS_KEY, sizeof(*stats), 0600 | IPC_CREAT);
    stats = shmat(stats_id, NULL, 0);

    /*definiamo i parametri se DENSE o LARGE, altrimenti li chiediamo in input*/
    #ifdef DENSE
        parameters->SO_capacity_min = 1;
        parameters->SO_capacity_max = 1;
        parameters->SO_timesec_min = 100000000; /*nsec*/
        parameters->SO_timesec_max = 300000000; /*nsec*/
        parameters->SO_timeout = 1; /*sec*/
        parameters->SO_duration = 20; /*sec*/
        parameters->SO_top_cells = 40;
        parameters->SO_holes = 10;
        parameters->SO_sources = (SO_WIDTH*SO_HEIGHT)-parameters->SO_holes;
        parameters->SO_taxies = parameters->SO_sources/2;
        parameters->SO_max_taxi = 0;
    #elif LARGE
        parameters->SO_capacity_min = 3;
        parameters->SO_capacity_max = 5;
        parameters->SO_timesec_min = 10000000; /*nsec*/
        parameters->SO_timesec_max = 100000000; /*nsec*/
        parameters->SO_timeout = 3; /*sec*/
        parameters->SO_duration = 20; /*sec*/
        parameters->SO_top_cells = 40;
        parameters->SO_holes = 50;
        parameters->SO_sources = 10;
        parameters->SO_taxies = 1000;
        parameters->SO_max_taxi = 0;
    #elif CUSTOM
        get_parameters(parameters);
        parameters->SO_max_taxi = 0;
    #endif

    /*inizializziamo le stats*/
    stats->travel_success = 0;
    stats->travel_aborted = 0;
    stats->travel_unfinished = 0;

    stats->taxi_top_request.n = 0;
    stats->taxi_top_request.pid = 0;
    stats->taxi_top_length_travel.n = 0;
    stats->taxi_top_length_travel.pid = 0;
    stats->taxi_top_crossed_cells.n = 0;
    stats->taxi_top_crossed_cells.pid = 0;

    
    position = 1;
    for (i = 0 ; i<SO_HEIGHT; i++){
        for (j = 0 ; j<SO_WIDTH ; j++){
            crossing_time_random = my_random(parameters->SO_timesec_min, parameters->SO_timesec_max, tempo);
            tempo++;
            max_n_taxi_random = my_random(parameters->SO_capacity_min, parameters->SO_capacity_max, tempo);
            parameters->SO_max_taxi += max_n_taxi_random;
            tempo++;

            shared_map -> matrix[i][j].isHole = 0;
            shared_map -> matrix[i][j].isSource = 0;
            shared_map -> matrix[i][j].crossed = 0;
            shared_map -> matrix[i][j].n_taxies = 0;
            shared_map -> matrix[i][j].crossing_time = crossing_time_random;
            shared_map -> matrix[i][j].max_n_taxi = max_n_taxi_random;
            shared_map -> matrix[i][j].pos = position;
            position++;
        }
    }
    printf("Empty map \n");
    print_map(shared_map);
    
    #ifdef CUSTOM
    /*HOLES*/
    printf("Insert number of holes: ");
    fgets(s_holes, sizeof(s_holes), stdin);
    parameters->SO_holes= atoi(s_holes);
    while(parameters->SO_holes < 0  || parameters->SO_holes > SO_WIDTH*SO_HEIGHT){
        printf ("Invalid parameter , try again with [0 < SO_HOLES < %d] : ", SO_WIDTH*SO_HEIGHT);
        fgets(s_holes, sizeof(s_holes), stdin);
        parameters->SO_holes= atoi(s_holes);
    }
    #endif

    switch (child_pid = fork()) {
        case -1 :
            TEST_ERROR;
            break ;
        case 0 :
            execve("holes",argv, NULL);
            break ;

        default :
            while(wait(NULL)!=-1);
            break ;
    }
    
    printf("Map with holes\n");
    print_map(shared_map);
    
    #ifdef CUSTOM
    /*TAXI*/
    printf("Insert number of taxi: ");
    fgets(s_taxi, sizeof(s_taxi), stdin);
    parameters->SO_taxies = atoi(s_taxi);

    while(parameters->SO_taxies > parameters->SO_max_taxi){
        printf("The number is over the max, you can insert max %d taxies \n",parameters->SO_max_taxi);
	    printf("Insert number of taxies:  ");
        fgets(s_taxi, sizeof(s_taxi), stdin);
        parameters->SO_taxies= atoi(s_taxi);
    }
    #endif

    for(i = 0; i< parameters->SO_taxies; i++){
        switch (child_pid = fork()) {
            case -1 :
                TEST_ERROR;
                break ;
            case 0 :
                execve("taxi",argv, NULL);
                break ;
            default :
                break ;
        }
    }

    myops.sem_num  = ID_TAXI;
    myops.sem_op = -parameters->SO_taxies;
    semop(s_id, &myops, 1);

    print_map(shared_map);

    #ifdef CUSTOM
    /*SOURCES*/
    printf("Insert number of sources [max %d sorces]:  ", ((SO_WIDTH * SO_HEIGHT)-shared_map->holes_in));
    fgets(s_sources, sizeof(s_sources), stdin);
    parameters->SO_sources = atoi(s_sources);

    while(parameters->SO_sources > ((SO_WIDTH * SO_HEIGHT)-shared_map->holes_in)){
        printf("The number is over the max, you can insert max %d sources \n", (SO_WIDTH*SO_HEIGHT)-shared_map->holes_in);
        printf("Insert number of sources: ");
        fgets(s_sources, sizeof(s_sources), stdin);
        parameters->SO_sources= atoi(s_sources);
    }
    #endif
    for(i = 0; i< parameters->SO_sources; i++){
        switch (child_pid = fork()) {
            case -1 :
                TEST_ERROR;
                break ;
            case 0 :
                execve("sources",argv, NULL);
                break ;
            default :
                break ;
        }
    }

    myops.sem_num  = ID_SOURCES;
    myops.sem_op = -parameters->SO_sources;
    semop(s_id, &myops, 1);

    printf("Start simulation\n");

    myops.sem_num  = ID_WAIT_FOR;
    myops.sem_op = -1;

    if(semop(s_id, &myops, 1)==-1){
        TEST_ERROR;
    }

    q_id = msgget(MSG_QUEUE_KEY, 0);    
    
    alarm(1);/*Ogni secondo stampo la mappa*/
    while(1);
    return 0;
    
}

void signalHandler(int sig){
    switch (sig){
    case SIGINT:/*DEALLOCA*/
        msgctl(q_id, IPC_RMID, NULL);
        semctl(s_id, IPC_RMID, 0);
        shmctl(m_id, IPC_RMID, 0);
        shmctl(p_id, IPC_RMID, 0);
        shmctl(stats_id, IPC_RMID, 0);
        exit(EXIT_SUCCESS);
        break;

    case SIGTERM:/*BACK DA TROPPI BUCHI - OK*/
        printf("I was able to insert only : %d \n", shared_map -> holes_in);
		break;

    case SIGUSR1:/*STAMPA STATS DOPO FINE SIMULAZIONE - OK*/
        printf("\n");
        while(wait(NULL)!= -1);
        printf("Final Map: \n");
        print_final_map(shared_map);
        top_cells = insert_top_cells(shared_map);
        print_top_cells(parameters->SO_top_cells, top_cells);

        printf("\n\n");
        printf("Completed travels: %d\n", stats->travel_success);
        printf("Aborted travels: %d\n", stats->travel_aborted);
        printf("Unfinished travels: %d\n", stats->travel_unfinished);
        printf("\n\n");
        printf("Taxi [%d] has traveled %d cells\n", stats->taxi_top_crossed_cells.pid ,stats->taxi_top_crossed_cells.n);
        printf("Taxi [%d] has completed %d requests\n", stats->taxi_top_request.pid ,stats->taxi_top_request.n);
        printf("Taxi [%d]  it took : %f ns to serve a request \n\n", stats->taxi_top_length_travel.pid ,(float) stats->taxi_top_length_travel.n);

        msgctl(q_id, IPC_RMID, NULL);
        semctl(s_id, IPC_RMID, 0);
        shmctl(m_id, IPC_RMID, 0);
        shmctl(p_id, IPC_RMID, 0);
        shmctl(stats_id, IPC_RMID, 0);
        exit(EXIT_SUCCESS);
        break;

    case SIGUSR2: /*REBORN TAXI - OK*/
        if(!fork()){
            execve("taxi_reborn", argv, NULL);
        }
        break;

    case SIGALRM : /*STAMPA MAPPA OGNI SEC - OK*/
        timer ++;
        if(timer != parameters->SO_duration){
            shm_acquire(s_id);
            printf("TIME: %d\n", timer);
            printf("Map update ... \n");
            print_map(shared_map);
            shm_release(s_id);
            alarm(1);
        }else{
            kill(0, SIGUSR1);
        }
        break;

    default:
        break;
    }
}