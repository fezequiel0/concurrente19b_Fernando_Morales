#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct{
    size_t error;
    size_t thread_count;
    size_t length;
    size_t** sudoku;
    size_t** errors;
    pthread_mutex_t error_mutex;
    pthread_barrier_t bloques;
    pthread_barrier_t filas;
    pthread_barrier_t columnas;
    pthread_barrier_t end;
} shared_data_t;

typedef struct {
    shared_data_t* shared_data;
    size_t thread_num; 
} private_data_t;

int create_sudoku(shared_data_t*);
void destroy_sudoku(shared_data_t*);
int create_threads(shared_data_t *);
void* run(void*);
void analyze_rows(private_data_t*);
void analyze_columns(private_data_t*);
void analyze_blocks(private_data_t*);
void print_errors(shared_data_t*, char);

int main(){
    shared_data_t* shared_data = calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 2;
	}
    scanf("%zu\n", &shared_data->length);
    shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    int error = create_sudoku(shared_data);
    if(error){
        return destroy_sudoku(shared_data),free(shared_data), 2;
    }
    pthread_barrier_init(&shared_data->bloques, NULL, shared_data->thread_count);
    pthread_barrier_init(&shared_data->filas, NULL, shared_data->thread_count);
    pthread_barrier_init(&shared_data->columnas, NULL, shared_data->thread_count);
    pthread_barrier_init(&shared_data->end, NULL, shared_data->thread_count);    
    pthread_mutex_init(&shared_data->error_mutex, NULL);
    error = create_threads(shared_data);
    if(error){
        return (void)fprintf(stderr, "ERROR: couldnt create threads\n"), 3;
    }
    if(!shared_data->error){
        printf("valid\n");
    }
    destroy_sudoku(shared_data);
    pthread_barrier_destroy(&shared_data->bloques);
    pthread_barrier_destroy(&shared_data->filas);
    pthread_barrier_destroy(&shared_data->columnas);
    pthread_barrier_destroy(&shared_data->end);
    pthread_mutex_destroy(&shared_data->error_mutex);
    free(shared_data);
    return 0;
}

int create_sudoku(shared_data_t* shared_data){
    shared_data->sudoku =calloc(shared_data->length * shared_data->length, sizeof(size_t*));
    for(size_t index = 0; index < (shared_data->length*shared_data->length); ++index){
        shared_data->sudoku[index] = calloc(shared_data->length*shared_data->length,sizeof(size_t));
    }        
    int error = 0;
    for(size_t temp_filas = 0; temp_filas < (shared_data->length*shared_data->length) && !error; ++temp_filas){
        for(size_t temp_columnas = 0; temp_columnas < (shared_data->length*shared_data->length) && !error; ++temp_columnas){
            int error_scanf = scanf("%zu ", &shared_data->sudoku[temp_filas][temp_columnas]);
            if(!error_scanf || error_scanf == EOF){
                ++error;
                fprintf(stderr, "e%zu,%zu\n", temp_filas+1, temp_columnas+1);
                shared_data->sudoku[temp_filas][temp_columnas] = 0;
            }
            else{
                size_t number = shared_data->sudoku[temp_filas][temp_columnas];
                if(number > shared_data->length * shared_data->length){
                    ++error;
                    fprintf(stderr, "e%zu,%zu\n", temp_filas+1, temp_columnas+1);
                    shared_data->sudoku[temp_filas][temp_columnas] = 0;
               }
            }
        }
        scanf("\n");    
    }
    shared_data->errors = calloc(shared_data->length * shared_data->length, sizeof(size_t*));
    for(size_t index = 0; index< (shared_data->length*shared_data->length); ++index){
        shared_data->errors[index] = calloc(shared_data->length*shared_data->length,sizeof(size_t));
    }
    if(!error){
        return 0;
    }
    else{
        return 1;
    }
}

void destroy_sudoku(shared_data_t * shared_data){
    for(size_t index = 0; index< (shared_data->length*shared_data->length); ++index){
        free(shared_data->sudoku[index]);
        free(shared_data->errors[index]);
    }
    free(shared_data->sudoku);
    free(shared_data->errors);    
}

int create_threads(shared_data_t * shared_data){
    private_data_t * private_data = (private_data_t*) calloc(shared_data->thread_count, sizeof(private_data_t));
    if(private_data == NULL){
        return 4;
    }
    pthread_t * threads = (pthread_t*)calloc(shared_data->thread_count,sizeof(pthread_t));
    if(threads == NULL){
        return free(private_data), 5;
    }
    for(size_t index = 0; index< shared_data->thread_count; ++index){
        private_data[index].shared_data = shared_data;
        private_data[index].thread_num = index;
        pthread_create(&threads[index], NULL, run, &private_data[index]);
    }
    for(size_t index = 0; ++index< shared_data->thread_count; ++index){
        pthread_join(threads[index], NULL);
    }
    free(private_data);
    free(threads);
    return 0;
}

void* run(void* data){
    private_data_t * private_data = (private_data_t*)data;
    shared_data_t * shared_data = private_data->shared_data;
    pthread_barrier_wait(&shared_data->filas);
    analyze_rows(private_data);
    pthread_barrier_wait(&shared_data->filas);
    if(private_data->thread_num == 0){
        print_errors(shared_data,'r');
    }
    pthread_barrier_wait(&shared_data->columnas);
    analyze_columns(private_data);
    pthread_barrier_wait(&shared_data->columnas);
    if(private_data->thread_num == 0){
        print_errors(shared_data,'c');
    }
    pthread_barrier_wait(&shared_data->bloques);
    analyze_blocks(private_data);
    pthread_barrier_wait(&shared_data->bloques);
    if(private_data->thread_num == 0){
        print_errors(shared_data,'b');
    }
    pthread_barrier_wait(&shared_data->end);
    return NULL;
}

void analyze_rows(private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t length = shared_data->length;
    for(size_t row = private_data->thread_num; row<(length*length); row+=shared_data->thread_count){
        for(size_t column = 0; column < (length*length); ++column){
            size_t num = shared_data->sudoku[row][column];
            if(num!=0){
                for(size_t column_to_compare = column+1; column_to_compare < (length*length); ++column_to_compare){
                    size_t num_to_compare = shared_data->sudoku[row][column_to_compare];
                    if(num == num_to_compare){
                        ++shared_data->errors[row][column_to_compare];
                        pthread_mutex_lock(&shared_data->error_mutex);
                        ++shared_data->error;
                        pthread_mutex_unlock(&shared_data->error_mutex);
                    }
                }
            }
        }    
    }
}
void analyze_columns(private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t length = shared_data->length;
    for(size_t column = private_data->thread_num; column<(length*length); column+=shared_data->thread_count){
        for(size_t row = 0; row < (length*length); ++row){
            size_t num = shared_data->sudoku[row][column];
            if(num!=0){
                for(size_t row_to_compare = row+1; row_to_compare < (length*length); ++row_to_compare){
                    size_t num_to_compare = shared_data->sudoku[row_to_compare][column];
                    if(num == num_to_compare){
                        ++shared_data->errors[row_to_compare][column];
                        pthread_mutex_lock(&shared_data->error_mutex);
                        ++shared_data->error;
                        pthread_mutex_unlock(&shared_data->error_mutex);

                    }
                }
            }
        }    
    }
}

void analyze_blocks(private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t length = shared_data->length;
    size_t numbers[length*length];        
    for(size_t block = private_data->thread_num; block < (length*length); block+=shared_data->thread_count){
        size_t row = block - (block % shared_data->length);
        size_t column = (block % shared_data->length) * shared_data->length;
        size_t end_row = row + length;
        size_t end_column = column + length;
        size_t index_to_insert = 0;
        for(size_t row_to_analyze = row ;row_to_analyze < end_row; ++row_to_analyze){
            for(size_t column_to_analyze = column; column_to_analyze < end_column; ++column_to_analyze){
                size_t num = shared_data->sudoku[row_to_analyze][column_to_analyze];
                int found_copy = 0;
                for(size_t index = 0; index < index_to_insert ; ++index){
                    if(num != 0){
                        if(num == numbers[index]){
                            ++shared_data->errors[row_to_analyze][column_to_analyze];
                            pthread_mutex_lock(&shared_data->error_mutex);
                            ++shared_data->error;
                            pthread_mutex_unlock(&shared_data->error_mutex);
                            found_copy = 1;
                        }
                    }
                }  
                if(!found_copy){
                    numbers[index_to_insert] = num;
                    ++index_to_insert;
                } 
            }            
        }

    }      
}

void print_errors(shared_data_t * shared_data, char type_of_error){
    for(size_t row = 0; row < shared_data->length * shared_data->length; ++row){
        for(size_t column = 0; column < shared_data->length * shared_data->length; ++column){
            if(shared_data->errors[row][column] != 0){
                printf("%c%zu,%zu\n",type_of_error, row+1,column+1);
                shared_data->errors[row][column] = 0;
            }
        }
    }
}