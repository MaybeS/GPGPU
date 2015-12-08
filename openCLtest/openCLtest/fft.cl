#pragma OPENCL EXTENSION cl_khr_icd  : enable
__kernel void fft1D_1024 (__global float2 *in, __global float2 *out,
                          __local float *sMemx, __local float *sMemy) {
  int tid = get_local_id(0);
  int blockIdx = get_group_id(0) * 1024 + tid;
  float2 data[16];

  // 전역 메모리 입출력 영역 시작 주소
  in = in + blockIdx;  out = out + blockIdx;

  globalLoads(data, in, 64); // 한 덩어리로 전역 메모리 읽기
  fftRadix16Pass(data);       // 자리 변경 없이 radix-16 처리
  twiddleFactorMul(data, tid, 1024, 0);

  // 지역 메모리를 이용한 지역 shuffle
  localShuffle(data, sMemx, sMemy, tid, (((tid & 15) * 65) + (tid >> 4)));
  fftRadix16Pass(data);               // 자리 변경 없이 radix-16 처리
  twiddleFactorMul(data, tid, 64, 4); // 회전 인수 곱셈

  localShuffle(data, sMemx, sMemy, tid, (((tid >> 4) * 64) + (tid & 15)));

  // radix-4 함수 호출 4회
  fftRadix4Pass(data);
  fftRadix4Pass(data + 4);
  fftRadix4Pass(data + 8);
  fftRadix4Pass(data + 12);

  //한덩어리로 전역 메모리에 기록
  globalStores(data, out, 64);
}