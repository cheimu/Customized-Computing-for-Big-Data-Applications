#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "xcl2.hpp"

using std::vector;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::string;
using std::sort;

void read_train_file(string filename,
    vector<unsigned long, aligned_allocator<unsigned long>> &train_image,
    int offset) {
  ifstream f;
  f.open(filename.c_str(), ios::in);
  if (!f.is_open()) {
    cout << "Open " << filename << " failed" << endl;
    exit(1);
  }

  int cnt = 0;
  while (!f.eof()) {
    string str;
    f >> str;
    unsigned long val = strtoul(str.substr(0, str.length() - 1).c_str(),
        NULL, 0);
    train_image[offset + cnt++] = val;
  }
  f.close();
  return ;
}

void read_test_file(string filename,
    vector<unsigned long, aligned_allocator<unsigned long>> &test_image,
    vector<int, aligned_allocator<int>> &test_label) {

  ifstream f;
  f.open(filename.c_str(), ios::in);
  if (!f.is_open()) {
    cout << "Open " << filename << " failed" << endl;
    exit(1);
  }
  int cnt = 0;
  while (!f.eof()) {
    string str;
    f >> str;
    unsigned long val = strtoul(str.substr(0, str.length() - 2).c_str(),
        NULL, 0);
    test_image[cnt] = val;
    int label = str[str.length() - 1] - '0';
    test_label[cnt++] = label;
  }
  f.close();
  return ;
}

int vote(vector<unsigned char, aligned_allocator<unsigned char>> &knn_mat) {
  int score[10] = {0};

  for (int i = 0; i < 30; i += 3)
    sort(knn_mat.begin() + i, knn_mat.begin() + i + 3);

  for (int i = 0; i < 3; ++i) {
    int m = INT_MAX;
    int id = 0;
    for (int j = 0; j < 10; ++j) {
      if (knn_mat[j * 3 + i] < m) {
        m = knn_mat[j * 3 + i];
        id = j;
      }
    }
    score[id]++;
  }
 
  int vid = 0;
  int vm = -1;
  for (int i = 0; i < 10; ++i) {
    if (score[i] > vm) {
      vm = score[i];
      vid = i;
    }
  }
  return vid;
}

