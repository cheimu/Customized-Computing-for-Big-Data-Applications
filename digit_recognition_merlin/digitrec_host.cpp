#include <iostream>
#include <string>

#define NUM_IMAGES 180

using std::cout;
using std::endl;
using std::string;
using std::to_string;

#ifdef MCC_ACC
#include MCC_ACC_H_FILE
#else
void digitrec_kernel(unsigned long test_image, unsigned long* train_images,
    unsigned char* knn_mat);
#endif

// Util functions for host
void read_train_file(string filename, unsigned long *train_image);
void read_test_file(string filename, unsigned long *test_image, int *test_label);
int vote(unsigned char *knn_mat);

int main(int argc, char **argv) {

  unsigned long *train_image = (unsigned long *)malloc(sizeof(unsigned long) * 18000);
  unsigned long *test_image = (unsigned long *)malloc(sizeof(unsigned long) * 180);
  unsigned char knn_mat[30];
  int *test_label = (int *)malloc(sizeof(int) * 180);

  if (argc < 2) {
    cout << "Usage: ./a.out <data path>" << endl;
    return 0;
  }

#ifdef MCC_ACC
  __merlin_init(argv[argc-1]);
#endif

  string path(argv[1]);

  // Prepare data
  for (int i = 0; i < 10; ++i)
    read_train_file(path + "/training_set_" + to_string(i) + ".dat",
        &train_image[i * 1800]);
  read_test_file(path + "/testing_set.dat", test_image, test_label);

  // Compute
  int correct = 0;
  for (int i = 0; i < NUM_IMAGES; ++i) {
#ifdef MCC_ACC
    __merlin_digitrec_kernel(test_image[i], train_image, knn_mat); 
#else
    digitrec_kernel(test_image[i], train_image, knn_mat);
#endif
    if (vote(knn_mat) == test_label[i]) 
      correct++;
  }
  cout << "Accuracy: " << 100.0 * ((float)correct / NUM_IMAGES) << "%" << endl;

#ifdef MCC_ACC
    __merlin_release();
#endif

  free(train_image);
  free(test_image);
  free(test_label);
  return 0;
}
