#include "functions.h"

#define VERBOSE

//genera numeri casuali

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





//stampa info platform


cl_int PrintPlatformInfo(cl_platform_id platform, cl_platform_info PlatInfo){	

//dichiarazione variabili
	cl_int err;
	size_t information_size;

//con la prima chiamata alla funzione clGetPlatformInfo memorizzo il numero di byte dell'info
	if(err=clGetPlatformInfo(platform,PlatInfo,0,NULL,&information_size)!=CL_SUCCESS){
			printf("errore nel ricevere l'informazione desiderata");
			return(err);			
	}
// alloco la quantita di spazio necessario pe memrorizzare le info 
	char *information=(char *)malloc(sizeof(char)*information_size);

//con la seconda chiamata alla funzione clGetPlatformInfo memorizzo l'info vera e propria
	if(err=clGetPlatformInfo (platform,PlatInfo,information_size,information,NULL)!=CL_SUCCESS){
		printf("errore nel ricevere l'informazione desiderata");
		return(err);	
	}
	
	printf("%s\n",information);
	free(information);

return(err);
}






//stampa info device

cl_int PrintDeviceInfo(cl_device_id device, cl_device_info DevInfo){	

//dichiarazione variabili
	cl_int err;
	size_t information_size;

//con la prima chiamata alla funzione clGetPlatformInfo memorizzo il numero di byte dell'info
	if(err=clGetDeviceInfo(device,DevInfo,0,NULL,&information_size)!=CL_SUCCESS){
			printf("errore nel ricevere l'informazione desiderata");
			return(err);			
	}
// alloco la quantita di spazio necessario pe memrorizzare le info 
	char *information=(char *)malloc(sizeof(char)*information_size);

//con la seconda chiamata alla funzione clGetPlatformInfo memorizzo l'info vera e propria
	if(err=clGetDeviceInfo (device,DevInfo,information_size,information,NULL)!=CL_SUCCESS){
		printf("errore nel ricevere l'informazione desiderata");
		return(err);	
	}
	
	printf("%s\n",information);
	free(information);

return(err);
}



//inizializza la platform

cl_int InitializePlatforms(cl_platform_id **platforms,cl_uint *num_platforms){
cl_int err;
int i;
	//con la prima chiamata a clGetPlatformIDs ottengo il numero di piattaforme
	if( err=clGetPlatformIDs (0,NULL,num_platforms)!=CL_SUCCESS){
		printf("errore nel creare le platform");
		return(err);	
	}
	*platforms=(cl_platform_id *)malloc(sizeof(cl_platform_id)*(*num_platforms));
	//richiamo ulteriormente la funzione clGetlatformIDs, questa volta memorizzando le platforms
	if(err=clGetPlatformIDs (*num_platforms,*platforms,NULL)!=CL_SUCCESS){
		printf("errore nel creare le platform");
		return(err);	
	}

#ifdef VERBOSE

	printf("Platforms info:\n");
	for(i=0;i<*num_platforms;i++){
		if(err=PrintPlatformInfo(*platforms[i],CL_PLATFORM_PROFILE)!=CL_SUCCESS){
			exit(0);	
		}

		if(err=PrintPlatformInfo(*platforms[i],CL_PLATFORM_VERSION)!=CL_SUCCESS){
			exit(0);	
		}

		if(err=PrintPlatformInfo(*platforms[i],CL_PLATFORM_NAME)!=CL_SUCCESS){
			exit(0);	
		}

		if(err=PrintPlatformInfo(*platforms[i],CL_PLATFORM_VENDOR)!=CL_SUCCESS){
			exit(0);	
		}
	
		if(err=PrintPlatformInfo(*platforms[i],CL_PLATFORM_EXTENSIONS)!=CL_SUCCESS){
			exit(0);	
		}
	printf("\n");
	}
#endif

return(err);
}



cl_int InitializeDevices(cl_platform_id platform,cl_device_id **devices,cl_uint *num_devices,size_t *nn){
cl_int err;
int j;

	
	if(err=clGetDeviceIDs(platform,CL_DEVICE_TYPE_ALL,0,NULL,num_devices)!=CL_SUCCESS){
		return(err);	
	}

	*devices=(cl_device_id *)malloc(sizeof(cl_device_id)*(*num_devices));

	if(err=clGetDeviceIDs(platform,CL_DEVICE_TYPE_ALL,*num_devices,*devices,NULL)!=CL_SUCCESS){
		return(err);	
	}



	for(j=0;j<*num_devices;j++){

#ifdef VERBOSE			

	printf("\n Device %d info: \n\n",j);
			err=PrintDeviceInfo(*devices[j],CL_DEVICE_NAME);
			err=PrintDeviceInfo(*devices[j],CL_DEVICE_VENDOR);
			err=PrintDeviceInfo(*devices[j],CL_DRIVER_VERSION);
			err=PrintDeviceInfo(*devices[j],CL_DEVICE_PROFILE);
			err=PrintDeviceInfo(*devices[j],CL_DEVICE_VERSION);
			err=PrintDeviceInfo(*devices[j],CL_DEVICE_OPENCL_C_VERSION);

			if(err!=CL_SUCCESS)
				return(err);	
	printf("\n");		
#endif



	cl_uint n1,n2;
	err=clGetDeviceInfo (*devices[j],CL_DEVICE_MAX_COMPUTE_UNITS ,(size_t)sizeof(cl_uint),(void *)&n1,NULL);

	if (err != CL_SUCCESS) {
		printf("Error in clGetDeviceInfo !!!Err code %d\n\n", err );
	}
	


	err=clGetDeviceInfo (*devices[j],CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,(size_t)sizeof(cl_uint),(void *)&n2,NULL);
	if (err != CL_SUCCESS) {
		printf("Error in clGetDeviceInfo !!!Err code %d\n\n", err );
	}

	

	
	err=clGetDeviceInfo (*devices[j],CL_DEVICE_MAX_WORK_GROUP_SIZE ,(size_t)sizeof(size_t),(void *)nn,NULL);
	if (err != CL_SUCCESS) {
		printf("Error in clGetDeviceInfo !!!Err code %d\n\n", err );
	}


	

	size_t nnn[n2];

	err=clGetDeviceInfo (*devices[j],CL_DEVICE_MAX_WORK_ITEM_SIZES ,(size_t)sizeof(size_t)*n2,(void *)&nnn,NULL);
	if (err != CL_SUCCESS) {
		printf("Error in clGetDeviceInfo !!!Err code %d\n\n", err );
	}
	

#ifdef VERBOSE
	printf("Device %d : maximum compute units = %d \n",j,n1);
	printf("Device %d : maximum work item dimension = %d \n",j,n2);
	printf("Device %d : maximum work group size = %d \n",j,*nn);

	int i;
	for(i=0;i<n2;i++)
		printf("Device %d : maximum work item %d size = %d \n",j,i,nnn[i]);
#endif 
	}


return(err);
}


void sort_test(int *array,size_t length){

int i;

for(i=0;i<length-1;i++)
	if(array[i]>array[i+1]){
		#ifdef VERBOSE
		printf("error : array not sorted!!!\n");
		printf("indice %d valore %d \n",i,array[i]);
		exit(0);
		#endif

	}

#ifdef VERBOSE
printf("\ntest ok!!\n\n");
#endif
}


