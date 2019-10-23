

#include <string.h>
#include <math.h>
#include <assert.h>

template <int n>
void Reduce(unsigned long* array) {
#pragma HLS inline
reduce:
  for (int i = 0; i < n; ++i) {
#pragma HLS unroll
    array[i] += array[i+n];
  }
}

void Load(const unsigned long* data_dram, const unsigned long test_image, unsigned long* data_local) {
#pragma inline off

  for (int i = 0; i < 1800; i++) {
    #pragma HLS pipeline
    data_local[i] = data_dram[i] ^ test_image;
  }
}

void Compute(const unsigned long* train_local, unsigned char* knn_mat, unsigned long* diss) {
#pragma inline off

compute:
  for (int y1 = 0; y1 < 1800; ++y1) {
    #pragma HLS pipeline
    // dis
  dis:
    for (int i = 0; i < 49; ++i) {
      #pragma HLS unroll
      diss[i] = (train_local[y1] & (1L<<i)) >> i;
    }

    Reduce<32>(diss);
    Reduce<16>(diss);
    Reduce<8>(diss);
    Reduce<4>(diss);
    Reduce<2>(diss);
    Reduce<1>(diss);

    unsigned long max_id = 0;
    // update
  update:
    for (int i = 0; i < 3; ++i) {
      #pragma HLS unroll
      if (knn_mat[max_id] < knn_mat[i]) {
        max_id = i;
      }
    }
    if (diss[0] < knn_mat[max_id]) {
      knn_mat[max_id] = diss[0];
    }
  }
}

extern "C" {

void digitrec_kernel(
    unsigned long test_image,
    unsigned long* train_images,
    unsigned char* knn_mat) {
#pragma HLS interface m_axi port=train_images offset=slave bundle=gmem
#pragma HLS interface m_axi port=knn_mat offset=slave bundle=gmem2
#pragma HLS interface s_axilite port=test_image bundle=control
#pragma HLS interface s_axilite port=train_images bundle=control
#pragma HLS interface s_axilite port=knn_mat bundle=control
#pragma HLS interface s_axilite port=return bundle=control

  unsigned int a;
#define BATCH_SIZE 360
  // Initialize the knn set
  unsigned long train_images_local[1800];
  unsigned char knn_mat_local[30];
#pragma HLS array_partition variable=train_images_local cyclic factor=360
#pragma HLS array_partition variable=knn_mat_local complete

init:
  for (int x = 0; x < 10; ++x) {
    #pragma HLS unroll
    #pragma HLS pipeline
    for (int y = 0; y < 3; ++y) {
      #pragma HLS unroll
      #pragma HLS pipeline
      // Note that the max distance is 49
      knn_mat_local[(y + (x * 3))] = (unsigned char)50;
    }
  }

  unsigned long test_image_local = test_image;
  
  unsigned long diss[64];
  #pragma HLS array_partition variable=diss complete
  for (int i = 49; i < 64; i++) {
    #pragma HLS unroll
    diss[i] = 0;
  }

// combine
combine:
  for (int x1 = 0; x1 < 10; ++x1) {
    // load data
    Load(train_images + x1 * 1800, test_image_local, train_images_local);
    Compute(train_images_local, knn_mat_local + x1 * 3, diss);
  }

load:
  for (int i = 0; i < 30; i++) {
   #pragma HLS unroll
   #pragma HLS pipeline
   knn_mat[i] = knn_mat_local[i];
 }

}

} // extern "C"
