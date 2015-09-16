#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__ || MACOSX
	#include <opencl/opencl.h>
	
#else
	#include <CL\opencl.h>
	#pragma warning ( disable : 4996 )

	#ifdef _WIN64
		#pragma comment (lib,"lib/x64/opencl.lib")
	#else
		#pragma comment (lib,"lib/x86/opencl.lib")
	#endif

#endif

#define NO_ERR (0)
#define MEM_SIZE (128)
#define BUFFER_SIZE (256)
#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char * argv[])
{
	cl_int err = NO_ERR, size;
	cl_platform_id platform;  
	cl_device_id * devices;
	cl_uint platfor_num, device_num;

	char buffer[BUFFER_SIZE];

	err += clGetPlatformIDs(1, &platform, &platfor_num);
	printf("openCL Platform detected: %d\n", platfor_num);

	err += clGetPlatformInfo(platform, CL_PLATFORM_NAME, BUFFER_SIZE, buffer, &size);
	printf("Platform name: %s\n", buffer);

	err += clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, BUFFER_SIZE, buffer, &size);
	printf("Platform profile: %s\n", buffer);

	err += clGetPlatformInfo(platform, CL_PLATFORM_VERSION, BUFFER_SIZE, buffer, &size);
	printf("Platform version: %s\n", buffer);

	err += clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, 0, &device_num);
	printf("Devices detected: %d\n", device_num);

	devices = (cl_device_id *)malloc(sizeof(cl_device_id) * device_num);
	err += clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, device_num, devices, &device_num);
	for (int device_index = 0; device_index < device_num; device_index++)
	{
		err += clGetDeviceInfo(devices[device_index], CL_DEVICE_NAME, 512, buffer, &size);
		printf("Device%d name: %s\n", device_index, buffer);
	}


	return 0;
}