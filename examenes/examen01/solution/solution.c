#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct{
    size_t thread_count;
    size_t length;
    size_t** sudoku;
    pthread_barrier_t bloques;
    pthread_barrier_t filas;
    pthread_barrier_t columnas;
    pthread_mutex_t stdout_mutex;
} shared_data_t;

typedef struct {
    shared_data_t* shared_data;
    size_t thread_num; 
} private_data_t;

size_t** create_sudoku(shared_data_t*);
void destroy_sudoku(size_t**, size_t);
int create_threads(shared_data_t *);
void* run(void*);
void analyze_rows(private_data_t*);
void analyze_columns(private_data_t*);
void analyze_blocks(private_data_t*);
size_t row_to_start(size_t, private_data_t*);
size_t column_to_start(size_t, private_data_t*);

int main(int argc, char* argv[]){
    shared_data_t* shared_data = calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 2;
	}
    if(argc>=2){
        sscanf(argv[1], "%zu", &shared_data->length);
        shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
        shared_data->sudoku = create_sudoku(shared_data);
        if(shared_data->sudoku == NULL){
            return free(shared_data),(void)fprintf(stderr,"ERROR: invalid sudoku"), 2;
        }
        pthread_barrier_init(&shared_data->bloques, NULL, shared_data->thread_count);
        pthread_barrier_init(&shared_data->filas, NULL, shared_data->thread_count);
        pthread_barrier_init(&shared_data->columnas, NULL, shared_data->thread_count);
        pthread_mutex_init(&shared_data->stdout_mutex, NULL);
        int error = create_threads(shared_data);
        if(error){
            return (void)fprintf(stderr, "ERROR: couldnt create threads"), 3;
        }
        destroy_sudoku(shared_data->sudoku, shared_data->length);
        pthread_barrier_destroy(&shared_data->bloques);
        pthread_barrier_destroy(&shared_data->filas);
        pthread_barrier_destroy(&shared_data->columnas);
        pthread_mutex_destroy(&shared_data->stdout_mutex);
    }
    else{
        printf("usage: length sudoku");
    }
    free(shared_data);
    return 0;
}

size_t** create_sudoku(shared_data_t* shared_data){
    size_t** sudoku =calloc(shared_data->length*shared_data->length, sizeof(size_t*));
    if(shared_data->sudoku){
        for(size_t index = 0; index < (shared_data->length*shared_data->length); ++index){
            sudoku[index] = calloc(shared_data->length*shared_data->length,sizeof(size_t));
        }        
    }
    else{
        return destroy_sudoku(sudoku, shared_data->length),NULL;
    }
    int error = 0;
    for(size_t temp_filas = 0; temp_filas < (shared_data->length*shared_data->length); ++temp_filas){
        for(size_t temp_columnas = 0; temp_columnas < (shared_data->length*shared_data->length); ++temp_columnas){
            size_t number = scanf("%zu", &number);
            if(number){
                sudoku[temp_filas][temp_columnas] = number;
            }
            else{
                ++error;
                fprintf(stderr, "e%zu%zu", temp_filas, temp_columnas);
                sudoku[temp_filas][temp_columnas] = 0;
            }
        }
        getchar();
    }
    if(!error){
        return sudoku;
    }
    else{
        return destroy_sudoku(sudoku, shared_data->length), NULL;
    }
}

void destroy_sudoku(size_t** sudoku, size_t length){
    for(size_t index = 0; index< (length*length); ++index){
        free(sudoku[index]);
    }
    free(sudoku);
}

int create_threads(shared_data_t * shared_data){
    private_data_t * private_data = (private_data_t*) calloc(shared_data->thread_count, sizeof(private_data_t));
    if(private_data == NULL){
        return 4;
    }
    for(size_t index = 0; index<shared_data->thread_count; ++index){
        private_data[index].shared_data = shared_data;
        private_data[index].thread_num = index;
    }
    pthread_t * threads = (pthread_t*)calloc(shared_data->thread_count,sizeof(pthread_t));
    if(threads == NULL){
        return free(private_data), 5;
    }
    for(size_t index = 0; index< shared_data->thread_count; ++index){
        pthread_create(&threads[index], NULL, run, &private_data[index]);
    }
    for(size_t index = 0; ++index< shared_data->thread_count; ++index){
        pthread_join(threads[index], NULL);
    }
    free(threads);
    free(private_data);
    return 0;
}

void* run(void* data){
    private_data_t * private_data = (private_data_t*)data;
    shared_data_t * shared_data = private_data->shared_data;
    pthread_barrier_wait(&shared_data->filas);
    analyze_rows(private_data);
    pthread_barrier_wait(&shared_data->columnas);
    analyze_columns(private_data);
    pthread_barrier_wait(&shared_data->bloques);
    analyze_blocks(private_data);
    return NULL;
}

void analyze_rows(private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t length = shared_data->length;
    for(size_t row = private_data->thread_num; row<(length*length); row+=private_data->thread_num){
        for(size_t column = 0; column < (length*length); ++column){
            size_t num = shared_data->sudoku[row][column];
            if(num!=0){
                for(size_t column_to_compare = column+1; column_to_compare < (length*length); ++column_to_compare){
                    size_t num_to_compare = shared_data->sudoku[row][column_to_compare];
                    if(num == num_to_compare){
                        pthread_mutex_lock(&shared_data->stdout_mutex);
                        printf("r%zu,%zu", row, column_to_compare);
                        pthread_mutex_unlock(&shared_data->stdout_mutex);
                    }
                }
            }
        }    
    }
}
void analyze_columns(private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t length = shared_data->length;
    for(size_t column = private_data->thread_num; column<(length*length); column+=private_data->thread_num){
        for(size_t row = 0; row < (length*length); ++row){
            size_t num = shared_data->sudoku[row][column];
            if(num!=0){
                for(size_t row_to_compare = row+1; row_to_compare < (length*length); ++row_to_compare){
                    size_t num_to_compare = shared_data->sudoku[row_to_compare][column];
                    if(num == num_to_compare){
                        pthread_mutex_lock(&shared_data->stdout_mutex);
                        printf("c%zu,%zu", row_to_compare, column);
                        pthread_mutex_unlock(&shared_data->stdout_mutex);
                    }
                }
            }
        }    
    }
}

void analyze_blocks(private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t length = shared_data->length;
    for(size_t block = private_data->thread_num; block<(length*length); block+=private_data->thread_num){
        size_t row = row_to_start(block,private_data);
        size_t column = column_to_start(block, private_data);
        size_t end_row = row + length;
        size_t end_column = column + length;
        size_t numbers[length*length];
        size_t index_to_insert = 0;
        for(size_t row_to_analyze = row ;row_to_analyze < end_row; ++row){
            for(size_t column_to_analyze = column; column_to_analyze < end_column; ++column){
                size_t num = shared_data->sudoku[row_to_analyze][column_to_analyze];
                int found_copy = 0;
                for(size_t index = 0; index < index_to_insert ; ++index){
                    if(num != 0){
                        if(num == numbers[index]){
                            pthread_mutex_lock(&shared_data->stdout_mutex);
                            printf("b%zu,%zu", row_to_analyze, column_to_analyze);
                            pthread_mutex_unlock(&shared_data->stdout_mutex);
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

size_t row_to_start(size_t block, private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t result = 0;
    result = block - (block % shared_data->length);
    return result;
}

size_t column_to_start(size_t block, private_data_t * private_data){
    shared_data_t * shared_data = private_data->shared_data;
    size_t result = 0;
    result = (block % shared_data->length) * shared_data->length;
    return result;
}