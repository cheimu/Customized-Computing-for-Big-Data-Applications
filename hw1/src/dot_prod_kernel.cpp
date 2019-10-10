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
  float local_c[4096];
  float res = 0;
#pragma HLS ARRAY_PARTITION variable=local_a block factor=128 dim=1
#pragma HLS ARRAY_PARTITION variable=local_b cyclic factor=128 dim=1
#pragma HLS ARRAY_PARTITION variable=local_c cyclic factor=128 dim=1
//#pragma HLS ARRAY_PARTITION variable=local_a complete dim=1
//#pragma HLS ARRAY_PARTITION variable=local_b complete dim=1  
  for (int i = 0; i < num_elems; i++) {
#pragma HLS UNROLL
#pragma HLS PIPELINE
    //float temp = local_a[i] * local_b[i];
    local_c[i] = local_a[i] * local_b[i];
    

#pragma HLS UNROLL
//#pragma HLS PIPELINE
   // res = res + temp;
   //res = res + local_c[i];    
  }
  
  //float res = 0;
  float ans[16] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
#pragma HLS ARRAY_PARTITION variable=ans complete dim=1
//#pragma HLS UNROLL
  LOOP_SPECIAL:for (int i = 0 ; i < 16; i++) {
#pragma HLS UNROLL
#pragma HLS PIPELINE
	for (int j = 0;j < 256; j++) {
#pragma HLS UNROLL
#pragma HLS PIPELINE
		ans[i] += local_c[i*256+j];
	}
   // ans[i] = ans[i] + local_c[i*256];
    //res = local_c[i] + res;
  }
  
  for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
#pragma HLS PIPELINE     
    res += ans[i];
  }
  
  *c = res;
}
  //*c = res;


}  // extern "C"
