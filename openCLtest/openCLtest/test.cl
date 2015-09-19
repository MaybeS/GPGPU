#pragma OPENCL EXTENSION cl_khr_icd  : enable

__kernel void DotProduct(__global float* a, __global float* b, __global float* c, int iNumElements)
{
	int iGID = get_global_id(0);

	if (iGID >= iNumElements)
		return;

	int iInOffset = iGID << 2;
	c[iGID] = a[iInOffset] * b[iInOffset] + a[iInOffset + 1] * b[iInOffset + 1] + a[iInOffset + 2] * b[iInOffset + 2] + a[iInOffset + 3] * b[iInOffset + 3];
}
__kernel void bilinear_interpolation_ind(
	__global float *source,
	__const int width,
	__const int height,
	__const float angle,
	__global float *target,
	__const int tw,
	__const int th)
{
	int xDest = get_global_id(0);
	int yDest = get_global_id(1);

	float fraction_x, fraction_y, one_minus_x, one_minus_y;

	if (xDest < tw && yDest < th) {
		int x = xDest - tw / 2;
		int y = th / 2 - yDest;

		float distance = sqrt((float)(x * x + y * y));
		float polarangle = 0.0f;
		if (x == 0) {
			if (y == 0) {
			}
			else if (y < 0) {
				polarangle = 1.5f * 3.14159265f;
			}
			else {
				polarangle = 0.5f * 3.14159265f;
			}
		}
		else {
			polarangle = atan2((float)y, (float)x);
		}

		polarangle -= angle;

		float truex = distance * cos(polarangle);
		float truey = distance * sin(polarangle);

		truex = truex + (float)(tw / 2);
		truey = (float)(th / 2) - truey;

		int floor_x = floor((float)(truex * (float)width / (float)tw));
		int floor_y = floor((float)(truey * (float)height / (float)th));
		int ceil_x = floor_x + 1;
		if (ceil_x >= width) ceil_x = floor_x;
		int ceil_y = floor_y + 1;
		if (ceil_y >= height) ceil_y = floor_y;

		if (floor_x < 0 || ceil_x < 0 || floor_x >= width || ceil_x >= width ||
			floor_y < 0 || ceil_y < 0 || floor_y >= height || ceil_y >= height) {
		}
		else {

			fraction_x = truex * (float)width / (float)tw - (float)floor_x;
			fraction_y = truey * (float)height / (float)th - (float)floor_y;
			one_minus_x = 1.0 - fraction_x;
			one_minus_y = 1.0 - fraction_y;

			float topleft = source[floor_y * width + floor_x];
			float topright = source[floor_y * width + ceil_x];
			float bottomleft = source[ceil_y * width + floor_x];
			float bottomright = source[ceil_y * width + ceil_x];

			float topvalue = one_minus_x * topleft + fraction_x * topright;

			float bottomvalue = one_minus_x * bottomleft + fraction_x * bottomright;

			float newvalue = one_minus_y * topvalue + fraction_y * bottomvalue;

			if (newvalue < 0.0) newvalue = 0.0;
			if (newvalue > 1.0) newvalue = 1.0;

			target[yDest * tw + xDest] = newvalue;
		}
	}
}