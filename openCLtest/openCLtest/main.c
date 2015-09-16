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

char * file_read(const char *, size_t *);


char * file_read(const char * path, size_t * size)
{
	FILE * file = fopen(path, "rb");
	char * file_text;

	fseek(file, 1, SEEK_END);
	(*size) = ftell(file);
	fseek(file, 0, SEEK_SET);

	file_text = (char*)malloc(sizeof(char) * (*size));
	fread(file_text, sizeof(char), (*size), file);
	fclose(file);

	return file_text;
}
int main(int argc, char * argv[])
{
	cl_int err = NO_ERR, size;
	cl_platform_id platform;  
	cl_device_id * devices;
	cl_uint platfor_num, device_num;

	size_t _index, _index__, _index___;

	char buffer[BUFFER_SIZE], _buffer[BUFFER_SIZE];

	err += clGetPlatformIDs(1, &platform, &platfor_num);
	printf("openCL Platform detected: %d\n", platfor_num);

	err += clGetPlatformInfo(platform, CL_PLATFORM_NAME, BUFFER_SIZE, buffer, &size);
	printf("Platform name: %s\n", buffer);

	err += clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, BUFFER_SIZE, buffer, &size);
	printf("Platform profile: %s\n", buffer);

	err += clGetPlatformInfo(platform, CL_PLATFORM_VERSION, BUFFER_SIZE, buffer, &size);
	printf("Platform version: %s\n", buffer);

	err += clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 0, 0, &device_num);
	printf("Devices detected: %d\n", device_num);

	devices = (cl_device_id *)malloc(sizeof(cl_device_id) * device_num);
	err += clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, device_num, devices, &device_num);
	for (_index = 0; _index < device_num; _index++)
	{
		err += clGetDeviceInfo(devices[_index], CL_DEVICE_NAME, BUFFER_SIZE, buffer, &size);
		printf("Device%d name: %s\n", _index, buffer);
	}

	if (err)
		return -1;

	cl_context context;
	cl_context_properties context_properties[] = { CL_CONTEXT_PLATFORM, platform, 0 };
	context = clCreateContext(context_properties, device_num, devices, NULL, NULL, &err);

	for (_index = 0; _index < device_num; _index++)
	{
		char device_name[BUFFER_SIZE];
		err += clGetDeviceInfo(devices[_index], CL_DEVICE_NAME, BUFFER_SIZE, device_name, &size);
		err += clGetDeviceInfo(devices[_index], CL_DEVICE_COMPILER_AVAILABLE, BUFFER_SIZE, buffer, (size_t*)&size);
		if(buffer[0] == CL_TRUE)
			printf("%s support kernel compile!\n", device_name);
		else
			printf("%s does not support kernel compile!\n", device_name);
	}

	if (err)
		return -1;

	char * kernel_source = file_read("./test.cl", &size);
	printf("%s", kernel_source);
	cl_program program = clCreateProgramWithSource(context, 1, &kernel_source, &size, &err);
	err += clBuildProgram(program, device_num, devices, NULL, NULL, NULL);

	cl_kernel kernel = clCreateKernel(program, "hello", &err);

	cl_mem memory = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, BUFFER_SIZE, buffer, &size);
	err += clSetKernelArg(kernel, 0, sizeof(cl_mem), &memory);

	cl_command_queue queue = clCreateCommandQueue(context, *devices, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);

	err += clEnqueueNDRangeKernel(queue, kernel, 1, 0, &size, 0, 0, 0, 0);
	err = clFinish(queue);


	return 0;
}