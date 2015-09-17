#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

__constant char hw[] = "Hello OpenCL!\n";

__kernel void hello(__global char * out)
{
    size_t tid = get_global_id(0);
    out[tid] = hw[tid];
}


__kernel void test (__global char * out)
{
	size_t uid = get_user_id(0);
	out [uid] = hw[uid];
}