#include <cmath>
#include <vector>
#include "xcl2.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char** argv)
{
  constexpr int kNumElems = 4096;

  //Allocate Memory in Host Memory
  size_t vector_size_bytes = sizeof(float) * kNumElems;

  vector<float, aligned_allocator<float>> source_in1(kNumElems);
  vector<float, aligned_allocator<float>> source_in2(kNumElems);
  vector<float, aligned_allocator<float>> source_hw_results(1);
  vector<float, aligned_allocator<float>> source_sw_results(1);

  source_hw_results[0] = 0.f;
  source_sw_results[0] = 0.f;

  // Create the test data and Software Result 
  for(int i = 0 ; i < kNumElems; ++i) {
    source_in1[i] = float(i) / kNumElems;
    source_in2[i] = float(i * i) / kNumElems / kNumElems;
    source_sw_results[0] += source_in1[i] * source_in2[i];
  }

//OPENCL HOST CODE AREA START
  vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  cl::Context context(device);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
  string device_name = device.getInfo<CL_DEVICE_NAME>(); 

  string binaryFile = xcl::find_binary_file(device_name, "dot_prod_kernel");
  cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
  devices.resize(1);
  cl::Program program(context, devices, bins);
  cl::Kernel kernel(program, "dot_prod_kernel");

  //Allocate Buffer in Global Memory
  vector<cl::Memory> inBufVec, outBufVec;
  cl::Buffer buffer_in1(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                        vector_size_bytes, source_in1.data());
  cl::Buffer buffer_in2(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
                        vector_size_bytes, source_in2.data());
  cl::Buffer buffer_output(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
                           sizeof(float), source_hw_results.data());
  inBufVec.push_back(buffer_in1);
  inBufVec.push_back(buffer_in2);
  outBufVec.push_back(buffer_output);

  //Copy input data to device global memory
  q.enqueueMigrateMemObjects(inBufVec, 0/* 0 means from host*/);

  auto krnl_vector_add = cl::KernelFunctor<cl::Buffer&, cl::Buffer&,
                                           cl::Buffer&, int>(kernel);

  //Launch the Kernel
  krnl_vector_add(cl::EnqueueArgs(q, cl::NDRange(1, 1, 1), cl::NDRange(1, 1, 1)),
                  buffer_in1, buffer_in2, buffer_output, kNumElems);

  //Copy Result from Device Global Memory to Host Local Memory
  q.enqueueMigrateMemObjects(outBufVec, CL_MIGRATE_MEM_OBJECT_HOST);
  q.finish();
//OPENCL HOST CODE AREA END
  
  // Compare the results of the Device to the simulation
  bool fail = false;
  if (fabs(source_hw_results[0] - source_sw_results[0]) /
      source_sw_results[0] > 1e-4) {
    cout << "Error: Result mismatch" << endl;
    cout << "CPU result = " << source_sw_results[0]
       << " Device result = " << source_hw_results[0] << endl;
    fail = true;
  }

  cout << "TEST " << (fail ? "FAILED" : "PASSED") << endl; 
  return (fail ? EXIT_FAILURE :  EXIT_SUCCESS);
}
