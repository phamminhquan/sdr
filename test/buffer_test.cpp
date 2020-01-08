#include <iostream>
#include <stdlib.h>
#include "../src/buffer.h"
#include <inttypes.h>
using namespace sdr;


int main() {
  
  /* instantiate a buffer
  size_t N = 10;
  auto buf = RawBuffer(N);
  for (size_t i=0; i<N; i++) {
    (buf._ptr+buf._offset)[i] = i;
  }

  // test buffer
  std::cout << "Print buffer entries:" << std::endl;
  for (size_t i=0; i<N; i++) {
    int x = (buf._ptr+buf._offset)[i];
    std::cout << x << std::endl;
  }

  // check inline functions
  std::cout << "Check inline functions:" << std::endl;
  char * buf_ptr = buf.ptr();
  int buf_data = buf_ptr[0]; 
  std::cout << buf_data << std::endl;
  buf._offset = 2;
  char * data_ptr = buf.data();
  int data = data_ptr[0];
  std::cout << data << std::endl;
  std::cout << buf.bytesOffset() << std::endl;
  std::cout << buf.bytesLen() << std::endl;
  std::cout << buf.storageSize() << std::endl;
  std::cout << buf.isEmpty() << std::endl;
  */

  // buffer type, and indexing
  Buffer<double> buf1(4);
  buf1[0] = 1;
  buf1[1] = 2;
  buf1[2] = 3;
  buf1[3] = 4;

  /*for (size_t i=0; i<4; i++) {
    std::cout << buf1[i] << std::endl;
  }
  */

  /* L1 and L2 norm
  std::cout << buf1.size() << std::endl;
  std::cout << buf1.norm() << std::endl;
  std::cout << buf1.norm2() << std::endl;
  */

  /*buf1 *= 2;
  for (size_t i=0; i<buf1.size(); i++)
    std::cout << buf1[i] << std::endl;
  buf1 /= 2;
  for (size_t i=0; i<buf1.size(); i++)
    std::cout << buf1[i] << std::endl;
  */

  //std::cout << buf1 << std::endl;

  // Test circular buffer
  std::cout << "Test circular buffer" << std::endl;
  CircularBuffer<double> cir_buf1(12);
  bool push_test = cir_buf1.push(buf1);
  if (push_test == true) { std::cout << "Push successful" << std::endl; }
  else { std::cout << "Push not successful" << std::endl; }
  
  // printing individual elements
  std::cout << "Print individual elements" << std::endl;
  for (size_t i=0; i<cir_buf1.size(); i++) {
    std::cout << cir_buf1[i] << std::endl;
  }
  
  // check inline functions
  std::cout << "Check byteslen(), bytesFree() inline functions" << std::endl;
  std::cout << "BytesLen: " << cir_buf1.bytesLen() << std::endl;
  std::cout << "BytesFree: " << cir_buf1.bytesFree() << std::endl;

  // printing pretty (havent implemented yet)
  //std::cout << cir_buf1 << std::endl;

  // test pull
  std::cout << "Testing pull method" << std::endl;
  Buffer<double> buf2(2);
  bool pull_test = cir_buf1.pull(buf2, 2);
  if (pull_test == true) { std::cout << "Pull successuful" << std::endl; }
  else { std::cout << "Pull Not Successful" << std::endl; }
  for (size_t i=0; i<cir_buf1.size(); i++) {
    std::cout << cir_buf1[i] << std::endl;
  }
  std::cout << "BytesLen: " << cir_buf1.bytesLen() << std::endl;
  std::cout << "BytesFree: " << cir_buf1.bytesFree() << std::endl;

  // test drop
  std::cout << "Test drop method" << std::endl;
  cir_buf1.drop(1);
  for (size_t i=0; i<cir_buf1.size(); i++) {
    std::cout << cir_buf1[i] << std::endl;
  }
  std::cout << "BytesLen: " << cir_buf1.bytesLen() << std::endl;
  std::cout << "BytesFree: " << cir_buf1.bytesFree() << std::endl;

  // test resize
  std::cout << "Test resize method" << std::endl;
  cir_buf1.resize(8);
  for (size_t i=0; i<cir_buf1.size(); i++) {
    std::cout << cir_buf1[i] << std::endl;
  }
  std::cout << "BytesLen: " << cir_buf1.bytesLen() << std::endl;
  std::cout << "BytesFree: " << cir_buf1.bytesFree() << std::endl;

  return 0;
}
