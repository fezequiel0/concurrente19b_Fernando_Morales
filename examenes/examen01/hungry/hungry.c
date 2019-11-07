#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

typedef struct {
    size_t rest_count;
    size_t* rest_capacity;
    size_t* rest_current_capacity;
    pthread_mutex_t* capacity_mutex;
    sem_t* rest_semaphore;
}shared_data_t;

typedef struct{
    shared_data_t * shared_data;
    size_t id;
}private_data_t;

void * patient(void*);
void * impatient(void*);
void walk();
void eat();

// main: 
int main(){
    shared_data_t * shared_data = calloc(1, sizeof(shared_data_t));
//     shared rest_count := read.integer()
    scanf("%zu ", &shared_data->rest_count);
//     shared rest_capacity[rest_count] := 0[rest_count]
    shared_data->rest_capacity = calloc(shared_data->rest_count, sizeof(size_t));
//     shared rest_current_capacity[rest_count] := 0[rest_count]
    shared_data->rest_current_capacity = calloc(shared_data->rest_count, sizeof(size_t));
//     shared capacity_mutex[rest_count]
    shared_data->capacity_mutex = calloc(shared_data->rest_count, sizeof(pthread_mutex_t));
//     shared rest_semaphore[rest_count]
    shared_data->rest_semaphore = calloc(shared_data->rest_count, sizeof(sem_t));
        
//     for local index := 0 to rest_count do   
//         rest_capacity[index] := read_integer()
//         capacity_mutex[index] := semaphore(1)
//         rest_semaphore[index] := semaphore(rest_capacity[index])
    for(size_t index = 0; index< shared_data->rest_count; ++index){
        scanf("%zu ", &shared_data->rest_capacity[index]);            
        pthread_mutex_init(&shared_data->capacity_mutex[index], NULL);
        sem_init(&shared_data->rest_semaphore[index], 0, shared_data->rest_capacity[index]);
    }
    private_data_t * private_data = calloc(1, sizeof(private_data_t));
    pthread_t * threads = calloc(1, sizeof(pthread_t));
//  local id := 0
    size_t id = 0;
    size_t stop = 0;
//  while true do
    while(!stop){//cambiar a sscanf metiendolo en un char y seguir mientras sea válido
//      case read_char() of 
        char choice = getchar();
   //      P : create_thread(patient(id))
        if(choice == 'P'){
            printf("LLEGA ACA CON %c", choice);

            private_data[id].id = id;

            printf("LLEGA ACA CON %c", choice);

            private_data[id].shared_data = shared_data;
            printf("LLEGA ACA CON %c", choice);
            pthread_create(&threads[id], NULL, patient, &private_data[id]);
            printf("LLEGA ACA CON %c", choice);
            ++id;
            private_data_t * new_private_data = realloc(private_data, (id+1)*sizeof(private_data_t));
            if(!new_private_data){
                --id;
                fprintf(stderr, "ERROR: couldnt allocate private data for %zu threads", id);
                choice = 'E';
            }
            private_data = new_private_data;
            pthread_t * new_threads = realloc(threads, (id+1)*sizeof(pthread_t));
            if(!new_threads){
                --id;
                fprintf(stderr, "ERROR: couldnt allocate private data for %zu threads", id);
                choice = 'E';
            }
            threads = new_threads;
        }
//      I : create_thread(impatient(id))
        if(choice == 'I'){
            private_data[id].id = id;
            private_data[id].shared_data = shared_data;
            pthread_create(&threads[id], NULL, impatient, &private_data[id]);
            ++id;
            private_data_t * new_private_data = realloc(private_data, (id+1)*sizeof(private_data_t));
            if(!new_private_data){
                --id;
                fprintf(stderr, "ERROR: couldnt allocate private data for %zu threads", id);
                choice = 'E';
            }
            private_data = new_private_data;
            pthread_t * new_threads = realloc(threads, (id+1)*sizeof(pthread_t));
            if(!new_threads){
                --id;
                fprintf(stderr, "ERROR: couldnt allocate private data for %zu threads", id);
                choice = 'E';
            }
            threads = new_threads;
        }
//      EOF: return
        if(choice != 'P' && choice != 'I'){
            ++stop;
        }
    }
    for(size_t index = 0; index< id; ++index){
        pthread_join(threads[index], NULL);
    }
    free(threads);
    free(private_data);
    free(shared_data->rest_capacity);
    free(shared_data->rest_current_capacity);
    for(size_t index = 0; index< shared_data->rest_count; ++index){
        pthread_mutex_destroy(&shared_data->capacity_mutex[index]);
        sem_destroy(&shared_data->rest_semaphore[index]);
    }
    free(shared_data->capacity_mutex);
    free(shared_data->rest_semaphore);
    free(shared_data);
    return 0;
}

// patient(id):
void* patient(void* data){
    private_data_t * private_data = (private_data_t*)data;
    shared_data_t * shared_data = private_data->shared_data;
    private_data->id = private_data->id % shared_data->rest_count;
//     wait(capacity_mutex[id])
    pthread_mutex_lock(&shared_data->capacity_mutex[private_data->id]);
//     rest_curent_capacity[id] := ++rest_current_capacity[id]
    shared_data->rest_current_capacity[private_data->id]+=1;
//     signal(capacity_mutex[id])
    pthread_mutex_unlock(&shared_data->capacity_mutex[private_data->id]);
//     wait(rest_semaphores[id])
    sem_wait(&shared_data->rest_semaphore[private_data->id]);
//     eat()
    eat();
//     signal(rest_semaphores[id])
    sem_post(&shared_data->rest_semaphore[private_data->id]);
//     wait(capacity_mutex[id])
    pthread_mutex_lock(&shared_data->capacity_mutex[private_data->id]);
//     rest_curent_capacity[id] := --rest_current_capacity[id]
    shared_data->rest_current_capacity[private_data->id]-=1;
//     signal(capacity_mutex[id])
    pthread_mutex_unlock(&shared_data->capacity_mutex[private_data->id]);
    return NULL;
}    

// impatient(id)
void * impatient(void * data){
    private_data_t * private_data = (private_data_t*)data;
//     rest_differences := 0[rest_count]
    shared_data_t * shared_data = private_data->shared_data;
    private_data->id = private_data->id % shared_data->rest_count;
    long long int rest_differences[shared_data->rest_count];
    size_t tries = 0;
    int didnt_eat = 1;
//     while tries < rest_count and didnt_eat = 1 do
    while(tries < shared_data->rest_count && didnt_eat){
//      wait(capacity_mutex[id])
        pthread_mutex_lock(&shared_data->capacity_mutex[private_data->id]);
        printf("LLEGA ACA CON %zu", private_data->id);
//      rest_differences[id] := rest_current_capacity[id] - rest_capacity[id]
        rest_differences[private_data->id] = (long long int)shared_data->rest_current_capacity[private_data->id] - (long long int)shared_data->rest_capacity[private_data->id];
//      signal(capacity_mutex[id])
        pthread_mutex_unlock(&shared_data->capacity_mutex[private_data->id]);
//      if differences[id] < 0 do
        if(rest_differences[private_data->id] < 0){
//             patient(id)
            patient(private_data);
//             didnt_eat := 0
            didnt_eat = 0;
        }
//         else do
        else{
//             ++id mod rest_count
            private_data->id = (private_data->id + 1)%shared_data->rest_count;
//             ++tries
            ++tries;
//             walk()
            walk();
        }

    }
//     if didnt_eat == 0 do
    if(didnt_eat == 0){
//         lowest_queue := differences[0]
        long long int lowest_queue = rest_differences[0]; 
//         destination_rest := 0
        size_t destination_rest = 0;
//         for rest_id := 0 to rest_count do
        for(size_t rest_id = 0; rest_id < shared_data->rest_count; ++rest_id){
//             if differences[rest_id] < lowest_queue do
            if(rest_differences[rest_id] < lowest_queue){
//              lowest_queue := differences[rest_id]
                lowest_queue = rest_differences[rest_id];
//                 destination_rest := rest_id
                destination_rest = rest_id;              
            }
        }
//         walk()
        walk();
//         patient(destination_rest)
        private_data->id = destination_rest;    
        patient(private_data);
    }
    return NULL;
}

void walk(){

}
void eat(){

}
