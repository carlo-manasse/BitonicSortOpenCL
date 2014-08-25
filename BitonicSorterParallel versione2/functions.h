#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <CL/cl.h>



//#define VERBOSE




int randomSequence(int * sequence, int size,int a,int b);
cl_int PrintPlatformInfo(cl_platform_id platform, cl_platform_info PlatInfo);
cl_int PrintDeviceInfo(cl_device_id device, cl_device_info DevInfo);
cl_int InitializePlatforms(cl_platform_id **platforms,cl_uint *num_platforms);
cl_int InitializeDevices(cl_platform_id platform,cl_device_id **devices,cl_uint *num_devices,size_t *nn);
void sort_test(int *array,size_t length);
