#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h> 
#include <unistd.h>


//#define steps
// compile this file with    gcc -o bitSorter_seq bitSorter_seq.c -lm



/********************************************************

The function randomSequence generates pseudo-random values
between a and b.
It takes in input four parameters:
- int * sequence is a pointer to an array
- int size is the size of the array
- int a is the lower bound
- int b is the upper bound

This function returns 1 if b<a, 0 otherwise.

********************************************************/

int randomSequence(int * sequence, int size,int a,int b){
	
	srand(time(0));
	if (b<a)
		return(1);
	
	else{
		int i;	
		for(i=0;i<size;i++){
			float num=(float) rand()/RAND_MAX;
			sequence[i]=a+(int)(num*(b-a));
		}
		return(0);
	}
}



/********************************************************

The function sum is part of the function bitSorter described
below. 

********************************************************/

int sum(int k,int blockSize,int distance){
	if((k+distance+1)%blockSize==0)
		return distance;
	else
		return 0;
}



/********************************************************

The function bitSorter implements an iterative version of
the bitonic sort algorithm.
It takes in input two parameters:
- int * sequence is a pointer to the array to be sorted
- int size is the size of the array, that must be a power
  of 2
This function returns nothing.

note:
If "steps" is defined bitSorter prints the output of all
the steps of the algorithm.

********************************************************/

void bitSorter(int * sequence, int size){

	int i,j,k,r;
	int log_size=(int) log2 ((double) size );

	#ifdef steps
	int count=1;
	#endif
		
	for (i=0;i<log_size;i++){
		
		for (j=i;j>-1;j--){
			int distance=(int)pow((double) 2,(double) j);
			int blockSize=(int)pow((double) 2,(double) (j+1));

			for(k=0;k<size;k=k+sum(k,blockSize,distance)+1){
				int c=k/((int)pow((double) 2,(double) (i+1)));
				int pari=c%2;

				if( ((int)(sequence[k]>sequence[k+distance]) + pari)==1 ) {	
					int tmp=sequence[k];
					sequence[k]=sequence[k+distance];
					sequence[k+distance]=tmp;
				}						
			}
			
			#ifdef steps
			printf(" passo %d \n",count);
			for(r=0;r<size;r++)
				printf(" %d ",sequence[r]);
			printf("\n");
			count++;
			#endif
		}	
	}

}



/********************************************************
The function below tests if the array is correclty sorted
in increasing order
********************************************************/

void sort_test(int *array,size_t length){

int i;

for(i=0;i<length-1;i++)
	if(array[i]>array[i+1]){
		printf("error : array not sorted!!!\n");
		printf("indice %d valore %d \n",i,array[i]);
		exit(0);

	}

printf("\ntest ok!!\n\n");
}




/********************************************************
Main fuction
********************************************************/


int main(int argc,char *argv[]){

int argument = 0;

int i;
int c;
opterr = 0;

	if ( argc < 4 || argc > 5 ){
		printf( "\nusage: %s N a b -options\n\n", argv[0] );
		printf( "-N is the dimension of the array to be sorted \n -a and b are the lower and upper bounds in which the random values are generated\n" );
		printf( "-option:\n" );
		printf( "-e the output is only the execution time\n" );
		printf( "-v verbose\n" );
		exit(0);	
	}

	if ((c = getopt (argc, argv, "ev")) != -1){
	switch (c) { 
		case 'e': argument = 1; break; 
		case 'v': argument = 2; break; 
		case '?': if (isprint (optopt)) fprintf (stderr, "Opzione non valida `-%c'.\n", optopt); 
			else fprintf (stderr, "Carattere non valido  `\\x%x'.\n", optopt); 
			return 1;
		default: abort (); 
		} 
		
		for (i = optind; i < argc; i++) printf ("l'opzione non esiste %s\n", argv[i]); 
	}





	//size of the array to be sorted
	int N = atoi(argv[1]);
	//lower and upper bound of the values in the array
	int a = atoi(argv[2]);
	int b = atoi(argv[3]);

	float log=log2((float)N);
	if(log - floor(log)!=0.0f){
	printf("\nerror: N must be a power of 2\n\n");
		exit(0);
	}
	

	//initialize and print the array 
	int * array=(int *)malloc(sizeof(int)*N);
	
	
	
	int err;

	if((err=randomSequence(array,N,a,b))!=0){
		printf("\nerror: a must be lower than b\n\n");
		exit(0);
	}


	//if verbose print the input
	if(argument==2){
		printf("\ninput \n\n");
		for(i=0;i<N;i++)
			printf(" %d ",array[i]);
		printf("\n\n\n");
	}

	
	//here is calculated the time elapsed to execute the sort

	struct timeval start,end;

	if(err=gettimeofday(&start, NULL)!=0)
		printf("error");


	//bitonic sort
	bitSorter(array,N);


	if(err=gettimeofday(&end, NULL)!=0)
		printf("error");


	long long elapsed = (end.tv_sec-start.tv_sec)*1000000LL + end.tv_usec-start.tv_usec;


	sort_test(array,N);

	//if -v print the output
	if(argument==2){
		printf("\n\nsorted array\n\n");
		for(i=0;i<N;i++)
			printf(" %d ",array[i]);
		printf("\n");
	}

	//if -v or -e print the elapsed time
	if(argument==2||argument==1){	
		printf("\n\nthe execution time is:  %f \n\n",(float) elapsed/1000000 );
		//printf("%f\n",(float) elapsed/1000000);
	}


return(0);
}
