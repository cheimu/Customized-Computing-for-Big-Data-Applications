#include <string.h>
#include <math.h>
#include <assert.h>


#pragma ACCEL kernel
void digitrec_kernel(
        unsigned long test_image,
        unsigned long* train_images,
        unsigned char* knn_mat) {

    
    #pragma ACCEL interface variable=train_images depth=18000 bus_bitwidth=512
    #pragma ACCEL interface variable=knn_mat depth=30
    unsigned char buf_knn_mat[30][3];

   #pragma ACCEL parallel 
   for (int x = 0; x < 30; ++x) {
        for (int y = 0; y < 3; ++y) {
            buf_knn_mat[x][y] = 50;
        }
    }

    #pragma ACCEL parallel
    for (int x = 0; x < 30; ++x) {
        #pragma ACCEL pipeline flatten
        for (int y = 0; y < 600; ++y) {
            unsigned long temp = train_images[x * 600 + y] ^ test_image;
            unsigned char dis = 0;
            #pragma ACCEL parallel reduction=dis factor=7
            for (int i = 0; i < 49; ++i) {
                dis += (temp & (1L << i)) >> i;
            }

            unsigned char max_id = 0;
            unsigned char max_val = 0;
            #pragma ACCEL pipeline flatten
            for (int i1 = 0; i1 < 3; ++i1) {
                if (max_val < buf_knn_mat[x][i1]) {
                    max_id = i1;
                    max_val = buf_knn_mat[x][i1];
                }
            }
            if (dis < max_val) {
                buf_knn_mat[x][max_id] = dis;
            }
        }
    }

    #pragma ACCEL parallel
    for (int iter = 0; iter < 10; iter++) {
        int x = iter*3;
        #pragma ACCEL pipeline flatten
        for (int y = 0; y < 3; ++y) {
          for (int i = 1; i < 3; i++) {
            unsigned char dis = buf_knn_mat[x+i][y];
            unsigned char max_id = 0;
            unsigned max_val = 0;
	    #pragma ACCEL pipeline flatten
            for (int i1 = 0; i1 < 3; ++i1) {
		if (max_val < buf_knn_mat[x][i1]) {
		   max_id = i1;
		   max_val = buf_knn_mat[x][i1];
                }
		
            }
            if (dis < max_val) {
                buf_knn_mat[x][max_id] = dis;
            }
          }
         }
    }


    #pragma ACCEL parallel
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 3; ++y) {
            knn_mat[x * 3 + y] = buf_knn_mat[3*x][y];
        }
    }
}
