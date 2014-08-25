#include "functions.h"
#define MAX_SOURCE_SIZE (0x100000)
#define TEST

int main(int argc,char *argv[]){
	
	int argument = 0;

	//indexes
	int i,j;

	int d;
	opterr = 0;

	if ( argc < 4 || argc > 5 ){
		printf( "\nusage: %s N a b -options\n\n", argv[0] );
		printf( "-N is the dimension of the array to be sorted \n -a and b are the lower and upper bounds in which the random values are generated\n" );
		printf( "-option:\n" );
		printf( "-e the output is only the execution time\n" );
		printf( "-r the output is only the transfer time\n" );
		printf( "-t the output is only the total time\n" );
		printf( "-a the output is the execution,transfer,total time\n" );
		printf( "-v verbose\n" );
		exit(0);	
	}

	if ((d = getopt (argc, argv, "evrat")) != -1){
	switch (d) { 
		case 'e': argument = 1; break; 
		case 'v': argument = 2; break;
		case 'r': argument = 3; break; 
		case 't': argument = 4; break;
		case 'a': argument = 5; break;  
 
		case '?': if (isprint (optopt)) fprintf (stderr, "Opzione non valida `-%c'.\n", optopt); 
			else fprintf (stderr, "Carattere non valido  `\\x%x'.\n", optopt); 
			return 1;
		default: abort (); 
		} 
		
		for (i = optind; i < argc; i++) printf ("l'opzione non esiste %s\n", argv[i]); 
	}



	//size of the array to be sorted
	size_t size=(size_t)atoi(argv[1]);

	//lower and upper bound of the values in the array
	int lower = atoi(argv[2]);
	int upper = atoi(argv[3]);

	
	

	//platform and device declarations
	cl_int err; 
	cl_uint num_platforms; 
	cl_platform_id *platforms;	
	cl_uint num_devices;
	cl_device_id *devices;
	cl_context context;
	cl_command_queue the_queue;	
	size_t nn;

	//calls to the functions that initialize the environment
	if(err=InitializePlatforms(&platforms,&num_platforms)!=CL_SUCCESS)
		exit(0);
	if(err=InitializeDevices(platforms[0],&devices,&num_devices,&nn)!=CL_SUCCESS)
		exit(0);

	//create the context
	context=clCreateContext (NULL,num_devices,devices,NULL,NULL,&err);
	if (err != CL_SUCCESS) {
		printf("Error in create context !!!Err code %d\n\n", err );
		exit(0);
	}	

	the_queue=clCreateCommandQueue(context,devices[0],CL_QUEUE_PROFILING_ENABLE,&err);
	if (err != CL_SUCCESS) {
		printf("Error in create command_queue !!!Err code %d\n\n", err );
		exit(0);
	}


	//initialize the array to be sorted with random numbers between lower and upper values
	int * a=(int *) malloc (sizeof(int)*size);
	err=randomSequence(a,size,lower,upper);


	if(argument==2){
	printf("\n");	
	printf("array to be sorted :\n");
	for(i=0;i<size;i++)
		printf(" %d ",a[i]);

	printf("\n");
	}

	//initialize the buffers that will be transferred to the device. mem_buffer_a is the array, mem_buffer_b contains the length of a and log_2 of size.
	cl_mem mem_buffer_a=clCreateBuffer (context,CL_MEM_READ_WRITE ,sizeof(int)*size,NULL,&err);
	if (err != CL_SUCCESS) {
		printf("Error in create buffer !!!Err code %d\n\n", err );
		exit(0);
	}

	int * b=(int *) malloc (sizeof(int)*4);
	int log_s=(int) log2 ((double) size );
	b[0]=(int)size;	
	b[1]=(int)log_s;
	
// b[0] -> size
// b[1] -> log size
// b[2] -> i
// b[3] -> j

	cl_mem mem_buffer_b=clCreateBuffer (context,CL_MEM_READ_ONLY ,sizeof(int)*4,NULL,&err);
	if (err != CL_SUCCESS) {
		printf("Error in create buffer !!!Err code %d\n\n", err );
		exit(0);
	}


	//here is opened the file in which the kernel is stored. 
	FILE *fp;
	char *kernel;
	size_t length;
    
 
	fp = fopen("kernel_bs.cl", "r");
	if (!fp) {
        	fprintf(stderr, "Failed to load kernel.\n");
        	exit(1);
    	}
	kernel = (char*)malloc(MAX_SOURCE_SIZE);
	length = fread( kernel, 1, MAX_SOURCE_SIZE, fp);
	fclose( fp );

	//initialize the program with the source in the kernel file
	cl_program the_program=clCreateProgramWithSource (context,1,(const char **)&kernel,&length,&err);

	if (err != CL_SUCCESS) {
		printf("Error in clCreateProgramWithSource !!!Err code %d\n\n", err );
		exit(0);
	}

	//compiling the kernel
	if(err=clBuildProgram (the_program,num_devices,devices,NULL,NULL,NULL)!=CL_SUCCESS){
		printf("compiling error\n");	
		size_t log_size;
		clGetProgramBuildInfo(the_program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char build_log [log_size+1];
		clGetProgramBuildInfo(the_program, devices[0], CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
		build_log[log_size] = '\0';
		printf("%s\n", build_log);
		exit(0);
	}

	
	//initialize the object the_kernel with the compiled kernel code, specifying the function to be called
	cl_kernel the_kernel=clCreateKernel (the_program,"BitonicSortParallel",&err);

	if (err != CL_SUCCESS) {
		printf("Error in clCreateKernel !!!Err code %d\n\n", err );
		exit(0);
	}


	//prepare the arguments 
	err=clSetKernelArg (the_kernel,0,(size_t)sizeof(cl_mem),(const void *)&mem_buffer_a);
	if (err != CL_SUCCESS) {
		printf("Error in clSetKernelArg 0 !!!Err code %d\n\n", err );
		exit(0);
	}

	err=clSetKernelArg (the_kernel,1,(size_t)sizeof(cl_mem),(const void *)&mem_buffer_b);
	if (err != CL_SUCCESS) {
		printf("Error in clSetKernelArg 1 !!!Err code %d\n\n", err );
		exit(0);
	}


	//calculate how many work items are needed
	size_t global;
	size_t local;


	global=(size_t)size/2;

	if (size>2*nn)	
		local=nn;
	else
		local=global;

if(argument==2)
	printf("number of global work items %d\n number of local work-items %d\n",global,local);

	cl_event event;
	cl_ulong start, end;


	err=clEnqueueWriteBuffer (the_queue,mem_buffer_a,CL_TRUE,0,(size_t) sizeof(int)*size,(void *)a,0,NULL,&event);
	if (err != CL_SUCCESS) {
		printf("Error in clEnqueueWriteBuffer !!!Err code %d\n\n", err );
		exit(0);
	}

	clFinish(the_queue);

	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling !!!Err code %d\n\n", err );
		exit(0);
	}

	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &start, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling !!!Err code %d\n\n", err );
		exit(0);
	}


	float transfertime=(end - start) * 1.0e-6f;

	
	//Put the kernel execution command in the queue

float kerneltime=0.0f;

for(i=0;i<log_s;i++){
	for (j=i;j>-1;j--){
		b[2]=i;
		b[3]=j;
		err=clEnqueueWriteBuffer (the_queue,mem_buffer_b,CL_TRUE,0,(size_t) sizeof(int)*4,(void *)b,0,NULL,&event);
		if (err != CL_SUCCESS) {
			printf("Error in clEnqueueWriteBuffer !!!Err code %d\n\n", err );
			exit(0);
		}


	clFinish(the_queue);

	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling !!!Err code %d\n\n", err );
		exit(0);
	}

	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &start, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling !!!Err code %d\n\n", err );
		exit(0);
	}

	transfertime=transfertime+(end - start) * 1.0e-6f;




		err=clEnqueueNDRangeKernel (the_queue,the_kernel,1,NULL,(const size_t *)&global,(const size_t 	*)&local,0,NULL,&event);
	
	
	if (err != CL_SUCCESS) {
		printf("Error in clEnqueueNDRangeKernel !!!Err code %d\n\n", err );
		exit(0);
	}

	//This command synchronizes the host and the device 
	clFinish(the_queue);

	//here is calculated the time elapsed to execute the kernel 
	
	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling 3 !!!Err code %d\n\n", err );
		exit(0);
	}

	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &start, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling 4 !!!Err code %d\n\n", err );
		exit(0);
	}

	kerneltime=kerneltime+(end - start) * 1.0e-6f;
}
}

	//read the buffer from the device
	err=clEnqueueReadBuffer (the_queue,mem_buffer_a,CL_TRUE,0,(size_t) sizeof(int)*size,(void *)a,0,NULL,&event);
	if (err != CL_SUCCESS) {
		printf("Error in clEnqueueReadBuffer !!!Err code %d\n\n", err );
		exit(0);	
	}

	//This command synchronizes the host and the device 
	clFinish(the_queue);




	
	//here is calculated the time elapsed to transfer data 
	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,sizeof(cl_ulong), &end, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling !!!Err code %d\n\n", err );
		exit(0);
	}

	err=clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,sizeof(cl_ulong), &start, NULL);
	if (err != CL_SUCCESS) {
		printf("Error in profiling !!!Err code %d\n\n", err );
		exit(0);
	}

	transfertime=transfertime + (end - start) * 1.0e-6f;




	float totaltime=transfertime+kerneltime;

if(argument==2){
	printf("\n");
	printf("sorted array: \n");
	for(i=0;i<size;i++)
		printf(" %d ",a[i]);

	printf("\n");
}


	//this function tests if the array is sorted correctly
#ifdef TEST
	sort_test(a,size);
#endif


if(argument==1){
	printf("\nkernel execution time (ms) %f \n\n",kerneltime);
	//printf("%f \n",kerneltime/1000);
}
if(argument==3){
	printf("\ntransfer time (ms) %f \n\n",transfertime);
	//printf("%f \n",transfertime/1000);
}
if(argument==4){
	printf("\ntotal time (ms) %f \n\n",totaltime);
	//printf("%f \n\n",totaltime/1000);
}

if(argument==2||argument==5){
	printf("\nkernel execution time - transfer time - total time (ms) \n\n");
	printf("%f	",kerneltime/1000);
	printf("%f	",transfertime/1000);
	printf("%f \n\n",totaltime/1000);
}



err=clReleaseContext (context);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}
err=clReleaseMemObject (mem_buffer_a);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}

err=clReleaseMemObject (mem_buffer_b);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}

err=clReleaseProgram (the_program);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}

err=clReleaseKernel (the_kernel);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}

err=clReleaseEvent (event);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}

err=clReleaseCommandQueue (the_queue);
if (err != CL_SUCCESS) {
		printf("Error in clReleaseContext !!!Err code %d\n\n", err );
		exit(0);	
	}


return(0);

}
