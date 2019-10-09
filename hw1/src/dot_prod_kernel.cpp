#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C" {

void dot_prod_kernel(const float* a, const float* b, float* c, const int num_elems) {
#pragma HLS interface m_axi port = a offset = slave bundle = gmem
#pragma HLS interface m_axi port = b offset = slave bundle = gmem
#pragma HLS interface m_axi port = c offset = slave bundle = gmem
#pragma HLS interface s_axilite port = a bundle = control
#pragma HLS interface s_axilite port = b bundle = control
#pragma HLS interface s_axilite port = c bundle = control
#pragma HLS interface s_axilite port = num_elems bundle = control
#pragma HLS interface s_axilite port = return bundle = control
#pragma HLS INLINE OFF  
  assert(num_elems <= 4096);  // this helps HLS estimate the loop trip count
  /***************************
   * your code goes here ... *
   ***************************/
  float local_a[4096];
  float local_b[4096];
  for (int i = 0; i < num_elems; i++) {
    local_a[i] = *a;
    local_b[i] = *b;
    a++;
    b++; 
  }
  //memcpy(&local_a[0], const_cast<float*>(a), sizeof(float)*4096);
  //memcpy(&local_b[0], const_cast<float*>(b), sizeof(float)*4096);  
  //float local_c[4096];
  float res = 0;
#pragma HLS ARRAY_PARTITION variable=local_a block factor=256 dim=1
#pragma HLS ARRAY_PARTITION variable=local_b block factor=256 dim=1
//#pragma HLS ARRAY_PARTITION variable=local_c block factor=4 dim=1
//#pragma HLS ARRAY_PARTITION variable=local_a complete dim=1
//#pragma HLS ARRAY_PARTITION variable=local_b complete dim=1  
  for (int i = 0; i < num_elems; i++) {
#pragma HLS UNROLL

    float temp = local_a[i] * local_b[i];
    
#pragma HLS PIPELINE
    res = res + temp;
  }
  
  //float res = 0;
/*  for (int i = 0 ; i < num_elems; i++) {
#pragma HLS PIPELINE
    res = local_c[i] + res;
  }
  *c = res;
}*/
  *c = res;
}

}  // extern "C"
