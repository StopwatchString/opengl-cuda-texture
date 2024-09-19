// kernel.cu
#include <cuda_runtime.h>

__global__ void helloKernel(char* str) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    // Change 'o' to '0'
    if (str[idx] == 'o') {
        str[idx] = '0';
    }
}

// Function declaration that will be called from C++
extern "C" void launchHelloKernel(char* str, int size);

// Implementation of the function that launches the kernel
void launchHelloKernel(char* str, int size) {
    char* d_str;
    cudaMalloc((void**)&d_str, size);
    cudaMemcpy(d_str, str, size, cudaMemcpyHostToDevice);

    helloKernel<<<1, size >>>(d_str);

    cudaMemcpy(str, d_str, size, cudaMemcpyDeviceToHost);
    cudaFree(d_str);
}