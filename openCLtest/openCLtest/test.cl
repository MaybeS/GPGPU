#pragma OPENCL EXTENSION cl_khr_icd  : enable

__kernel void DotProduct(__global float* a, __global float* b, __global float* c, int iNumElements)
{
	// 코어의 인덱스를 얻어온다.(이 코어가 작업할 영역을 구한다)
	int iGID = get_global_id(0);

	// 위에서 보면 global worksize를 element보다 같거나 더 많이 잡았다. element의 갯수보다 인덱스가 높을경우 아무것도 하지 말고 끝내는 부분
	if (iGID >= iNumElements)
	{
		return;
	}

	// process
	int iInOffset = iGID << 2;
	c[iGID] = a[iInOffset] * b[iInOffset]
		+ a[iInOffset + 1] * b[iInOffset + 1]
		+ a[iInOffset + 2] * b[iInOffset + 2]
		+ a[iInOffset + 3] * b[iInOffset + 3];
}