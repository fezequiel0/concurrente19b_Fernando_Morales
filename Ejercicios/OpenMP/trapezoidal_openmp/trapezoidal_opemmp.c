#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

void trapezoidal_area(double,double,double,size_t,int,double*);
double witch_of_agnesi(double);

int main(int argc, char*argv[]){
    if(argc >= 4){
        double a = strtod(argv[1], NULL);
        if(!a){
            fprintf(stderr, "ERROR: invalid bottom_limit\n");
	    	return 2;
        }
        double b = strtod(argv[2], NULL);
        if(!b){
            fprintf(stderr, "ERROR: invalid upper_limit\n");
	    	return 3;
        }        
        size_t n = strtoull(argv[3],NULL,10);    
        if(!n){
            fprintf(stderr, "ERROR: invalid number_of_trapezoids\n");
	    	return 4;
        }
        int number_of_threads = omp_get_max_threads();
        if(argc > 4){
            number_of_threads = atoi(argv[4]);
        }        
        struct timespec start_time;
		clock_gettime(CLOCK_MONOTONIC, &start_time);
        double resultado = 0;
        double * result = &resultado;
        trapezoidal_area(1,a,b,n, number_of_threads, result);
        struct timespec finish_time;
        clock_gettime(CLOCK_MONOTONIC, &finish_time);
        double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
        printf("Result : %f\n", resultado);
        printf("Simulation time %.9lf s\n", elapsed_seconds);
	}
    else{
        fprintf(stderr, "usage: bottom_limit upper_limit number_of_trapezoids number_of_threads\n");
		return 1;
	}
    return 0;
}

void trapezoidal_area(double f,double a, double b, size_t n, int number_of_threads, double * resultado){
    (void)f;
    double sum = (b-a)/(double)n;
    double result_temp = 0;
    #pragma omp	parallel for num_threads(number_of_threads) default(none) shared(resultado,a,b,sum,n) reduction(+:result_temp)    
    for(size_t iteration = 0; iteration<n-1;++iteration){
        result_temp += sum*((witch_of_agnesi(iteration*sum+a)+witch_of_agnesi(((iteration+1)*sum)+a))/((double)2));
    }
    #pragma omp critical(resultado)
        *(resultado) += result_temp;
}

double witch_of_agnesi(double x){
    double a = 3;
    return a*a*a / (x*x + a*a);
}