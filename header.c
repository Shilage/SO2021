
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


#include "header.h"
int m, n, i, j, q;

int my_random(int min, int max, int seed){
    srand(seed);
    return (rand() % (max-min+1)) + min;
}

/*Stampa lo stato di occupazione delle celle*/
void print_map(struct map * map){
    for(m = 0; m<SO_HEIGHT; m++){
        for(n = 0; n<SO_WIDTH; n++){
            if(map->matrix[m][n].isHole == 0){
                if(map->matrix[m][n].isSource){
                    printf("[%d, S]\t" , map->matrix[m][n].n_taxies);
                }else{
                    printf("[%d, N]\t" , map->matrix[m][n].n_taxies);
                }
            }else{
                printf("[HOLE]\t");
            }
        }
        printf("\n\n");
    }
}

void print_final_map(struct map * map){
    for(m = 0; m<SO_HEIGHT; m++){
        for(n = 0; n<SO_WIDTH; n++){
            if(map->matrix[m][n].isHole == 0){
                if(map->matrix[m][n].isSource){
                    printf("[S]\t");
                }else{
                    printf("[-]\t");
                }
            }else{
                printf("[H]\t");
            }
        }
        printf("\n\n");
    }
}

cell * insert_top_cells (struct map* shared_map ){
    static cell top_cells [SO_HEIGHT* SO_WIDTH];
    int q = 0;
    for(i = 0; i < SO_HEIGHT; i++){
        for(j = 0; j < SO_WIDTH; j++){
            top_cells[q].crossed = shared_map->matrix[i][j].crossed;
            top_cells[q].pos = shared_map->matrix[i][j].pos;
            q++;
        }
    }
    sort(top_cells);
    return top_cells;
}
void sort(cell* a){
    int p,r;
    cell temp;
    for(p = 0; p < SO_WIDTH*SO_HEIGHT-1; p++){
        for(r = p+1; r<SO_WIDTH*SO_HEIGHT; r++){
            if(a[r].crossed>a[p].crossed){
                temp.crossed = a[p].crossed;
                temp.pos = a[p].pos;
                a[p].crossed = a[r].crossed;
                a[p].pos = a[r].pos;
                a[r].crossed = temp.crossed;
                a[r].pos = temp.pos;
            }
        }
    }
}
void print_top_cells(int dim, cell* top){
    for(i = 0; i<dim; i++){
        printf("%d) Cell in position %d, crossed: %d times\n", i+1, top[i].pos, top[i].crossed);
    }
}
void get_parameters(struct param* parameters){
    char top_cells[10], cap_min[10], cap_max[10], timesec_min[10], timesec_max[10], duration[10], timeout[10];
    
    /*SO_DURATION*/
    printf("Insert duration of the simulation [sec]: ");
    fgets(duration, sizeof(duration), stdin);
    parameters->SO_duration = atoi(duration);
    while (parameters->SO_duration < 0){
        printf ("Invalid parameter, try again: [0 < SO_duration]: ");
        fgets(duration, sizeof(duration), stdin);
        parameters->SO_duration= atoi(duration);
    }

    /*SO_TIMEOUT*/
    printf("Insert timeout of every taxies [sec]: ");
    fgets(timeout, sizeof(timeout), stdin);
    parameters->SO_timeout = atoi(timeout);
    while (parameters->SO_timeout < 0){
        printf ("Invalid parameter, try again: [0 < SO_timeout]: ");
        fgets(timeout, sizeof(timeout), stdin);
        parameters->SO_timeout= atoi(timeout);
    }

    /*TOP_CELLS*/
    printf("Insert SO_TOP_CELLS: ");
    fgets(top_cells, sizeof(top_cells), stdin);
    parameters->SO_top_cells = atoi(top_cells);
    while (parameters->SO_top_cells < 0 || parameters->SO_top_cells > SO_WIDTH*SO_HEIGHT){
        printf ("Invalid parameter, try again: [0 < so_top_cells < number of cells]: ");
        fgets(top_cells, sizeof(top_cells), stdin);
        parameters->SO_top_cells= atoi(top_cells);
    }

    /*SO_CAP_MIN*/
    printf("Insert SO_CAP_MIN: ");
    fgets(cap_min, sizeof(cap_min), stdin);
    parameters->SO_capacity_min = atoi(cap_min);
    while (parameters->SO_capacity_min < 1){
        printf ("Invalid parameter, try again: [0 < SO_CAP_MIN]: ");
        fgets(cap_min, sizeof(cap_min), stdin);
        parameters->SO_capacity_min= atoi(cap_min);
    }

    /*SO_CAP_MAX*/
    printf("Insert SO_CAP_MAX: ");
    fgets(cap_max, sizeof(cap_max), stdin);
    parameters->SO_capacity_max = atoi(cap_max);
    while (parameters->SO_capacity_max < parameters->SO_capacity_min){
        printf ("Invalid parameter, try again: [SO_CAP_MIN <= SO_CAP_MAX]: ");
        fgets(cap_max, sizeof(cap_max), stdin);
        parameters->SO_capacity_max= atoi(cap_max);
    }

    /*SO_TIMESEC_MIN*/
    printf("SO_TIMESEC_MIN to be converted in nanosec  \n(1 = 0,01 sec = 10000000ns)\n(10 = 0,1 sec = 100000000ns) \nInsert SO_TIMESEC_MIN: ");
    fgets(timesec_min, sizeof(timesec_min), stdin);
    parameters->SO_timesec_min = atoi(timesec_min);
    while (parameters->SO_timesec_min < 1 || parameters->SO_timesec_min > 1000){
        printf ("Invalid parameter , try again: [1 <= SO_TIMESEC_MIN < 1000]: ") ;
        fgets(timesec_min, sizeof(timesec_min), stdin);
        parameters->SO_timesec_min= atoi(timesec_min);
    }
    
     /*SO_TIMESEC_MAX*/
    printf("SO_TIMESEC_MAX to be converted in nanosec  \n(1 = 0,01 sec = 10000000ns)\n(10 = 0,1 sec = 100000000ns) \nInsert SO_TIMESEC_MAX: ");
    fgets(timesec_max, sizeof(timesec_max), stdin);
    parameters->SO_timesec_max= atoi(timesec_max);
    while (parameters->SO_timesec_max < parameters->SO_timesec_min || parameters->SO_timesec_max > 1000){
        printf ("Invalid parameter , try again: [SO_TIMESEC_MIN <= SO_TIMESEC_MAX < 1000]: ");
        fgets(timesec_max, sizeof(timesec_max), stdin);
        parameters->SO_timesec_max= atoi(timesec_max);
    }

    parameters->SO_timesec_min = parameters->SO_timesec_min * 10000000;
    parameters->SO_timesec_max = parameters->SO_timesec_max * 10000000;
}
