#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

double trapezoidal_area(double,double,double,size_t);
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
        struct timespec start_time;
		clock_gettime(CLOCK_MONOTONIC, &start_time);
        double resultado = trapezoidal_area(1,a,b,n);
        struct timespec finish_time;
        clock_gettime(CLOCK_MONOTONIC, &finish_time);
        double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
        printf("Result : %f\n", resultado);
        printf("Simulation time %.9lf s\n", elapsed_seconds);
	}
    else{
        fprintf(stderr, "usage: bottom_limit upper_limit number_of_trapezoids\n");
		return 1;
	}
    return 0;
}

double trapezoidal_area(double f,double a, double b, size_t n){
    (void)f;
    double resultado = 0;
    double sum = (b-a)/(double)n;        
    for(double index = a; index < b; index+=sum){
        resultado += sum*((witch_of_agnesi(index)+witch_of_agnesi(index+sum))/((double)2));
    }
    return resultado;
}

double witch_of_agnesi(double x){
    double a = 3;
    return a*a*a / (x*x + a*a);
}