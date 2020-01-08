#ifndef __SDR_FIFO_H__
#define __SDR_FIFO_H__

#include <ostream>
#include <complex>
#include <inttypes.h>
#include <math.h>
#include <cstring>

namespace sdr {
  
  template <class T> 
  class Fifo: public queue {
    public:
      // Empty contructor
      Buffer() : RawBuffer(), _size(0) 
      {}

      // Constructor with size N
      Buffer(size_t N) : RawBuffer(N*sizeof(T)), _size(N)
      {}

      // Contructor from data
      Buffer(T* data, size_t size)
        : RawBuffer((char*)data, 0, sizeof(T)*size), _size(size)
      {}

      // Construct from another buffer (new reference)
      Buffer(const Buffer<T> &other)
        : RawBuffer(other), _size(other._size)
      {}

      // Destructor
      virtual ~Buffer() {
        _size = 0;
      }

      // Explicit type cast
      explicit Buffer(const RawBuffer &other)
        : RawBuffer(other), _size(_length/sizeof(T))
      {}
      
    public:
      // OPERATOR OVERLOAD
      // Assignment operator
      const Buffer<T> &operator = (const Buffer<T> other) {
        RawBuffer::operator =(other);
        _size = other._size;
        return *this;
      }
      
      // Return Size
      inline size_t size() const { return _size; }

      // Comparison
      inline bool operator< (const Buffer<T> &other) const {
        return this->_ptr < other._ptr;
      }
      
      // TODO: Implement Exception handling out of bounds
      /*inline T &operator[] (int index) const {
        if ((index >= _size) || (index < 0)) {
          RuntimeError err;
          err << "Index " << index << " out of bounds [0, " << _size << ")";
          throw err;
        }
        return reinterpret_cast<T*>(_ptr+_offset)[index];
      }*/

      // Indexing
      inline T &operator[] (int index) const {
        /*if ((index >= _size) || (index < 0)) {
          RuntimeError err;
          err << "Index " << index << " out of bounds [0, " << _size << ")";
          throw err;
        }*/
        return reinterpret_cast<T*>(_ptr+_offset)[index];
      }

      // L1 norm
      inline double norm_l1() const {
        double nrm = 0;
        for (size_t i=0; i<size(); i++)
          nrm += std::abs((*this)[i]);
        return nrm;
      }

      // L2 norm
      inline double norm_l2() const {
        double nrm2 = 0;
        for (size_t i=0; i<size(); i++)
          nrm2 += std::real(std::conj((*this)[i])*(*this)[i]);
        return std::sqrt(nrm2);
      }

      // LP norm
      inline double norm_lp(double p) const {
        double nrm = 0;
        for (size_t i=0; i<size(); i++)
          nrm += std::pow(std::abs((*this)[i]), p);
        return std::pow(nrm, 1./p);
      }

      // In-place Element-wise product of buffer with scalar
      inline Buffer<T> &operator *= (const T &a) {
        for (size_t i=0; i<size(); i++)
          (*this)[i] *= a;
        return *this;
      }

      // In-place Element-wise division with scalar
      inline Buffer<T> &operator /= (const T &a) {
        for (size_t i=0; i<size(); i++)
          (*this)[i] /= a;
        return *this;
      }
    
      // Explicit Type Cast
      template <class oT>
      Buffer<oT> as() const {
        return Buffer<oT>((const RawBuffer &)(*this));
      }
      
      // Return new view on this buffer
      inline Buffer<T> sub(size_t offset, size_t len) const {
        if ((offset+len) > _size) { return Buffer<T>(); }
        return Buffer<T>(RawBuffer(*this, offset*sizeof(T), len*sizeof(T)));
      }

      // Return head view
      inline Buffer<T> head(size_t n) const {
        if (n > _size) { return Buffer<T>(); }
        return this->sub(0,n);
      }

      // Return tail view
      inline Buffer<T> tail(size_t n) const {
        if (n > _size) { return Buffer<T>(); }
        return this->sub(_size-n, n);
      }

    protected:
      // number of entry in buffer
      size_t _size;

  };

  // Template class with operator overloading to print buffer pretty
  template <class Scalar>
  std::ostream &
  operator << (std::ostream &stream, const sdr::Buffer<Scalar> &buffer) {
    stream << "[";
    if (10 < buffer.size()) {
      stream << +buffer[0];
      for (size_t i=1; i<5; i++)
        stream << ", " << +buffer[i];
      stream << ", ..., " << +buffer[buffer.size()-6];
      for (size_t i=1; i<5; i++)
        stream << ", " << +buffer[buffer.size()+i-6];
    } else {
      if (0 < buffer.size()) {
        stream << +buffer[0];
        for (size_t i=1; i<buffer.size(); i++)
          stream << ", " << +buffer[i];
      }
    }
    stream << "]";
    return stream;
  }

  // Circular Buffer
  class RawCircularBuffer: public RawBuffer {
    public:
      // Empty constructor
      RawCircularBuffer();

      // Construct from size
      RawCircularBuffer(size_t size);
      
      // Construct from another buffer
      RawCircularBuffer(const RawCircularBuffer &other);

      // virtual destructor
      virtual ~RawCircularBuffer();

      // Operator Overloading
      // Assignment overloading
      inline const RawCircularBuffer &operator = (const RawCircularBuffer &other) {
        RawBuffer::operator = (other);
        _take_index = other._take_index;
        _b_stored = other._b_stored;
        return *this;
      }

      // Element access
      // TODO: exception handling for out of bounds indexing
      char &operator[] (int index) {
        int i = _take_index+index;
        if (i >= (int)_storage_size) { i -= _storage_size; } // wrap back if out of bounds
        return *(ptr()+i);
      }

      // Return number of bytes available for reading
      inline size_t bytesLen() const { return _b_stored; }

      // Return number of free bytes
      inline size_t bytesFree() const { return _storage_size-_b_stored; }

      // push given data in buffer, size of given data must be smaller of equal to
      // the number of free bytes. If so, return true. If not, return false
      inline bool push(const RawBuffer &src) {
        // if given data is larger than number of free bytes
        if (src.bytesLen() > bytesFree()) { return false; }
        size_t put_index = _take_index+_b_stored;
        if (put_index > _storage_size) { put_index -= _storage_size; } // wrap around
        // store data
        if (_storage_size >= (put_index+src.bytesLen())) {
          // if data can be copied directly
          std::memcpy(_ptr+put_index, src.data(), src.bytesLen());
          _b_stored += src.bytesLen();
          return true;
        }
        // if wrapped around then store first half
        size_t num_a = _storage_size-put_index;
        std::memcpy(_ptr+put_index, src.data(), num_a); // first few bytes
        std::memcpy(_ptr, src.data()+num_a, src.bytesLen()-num_a); // the wrap around bytes
        _b_stored += src.bytesLen();
        return true;
      }

      // pull N number of bytes from circular buffer and store it into a given 
      // destination. Return true if data is pulled successfully
      inline bool pull(const RawBuffer &dest, size_t N) {
        // check to see if destination buffer is big enough
        if (N > dest.bytesLen()) { return false; } // if destination buffer is not big enough
        if (N > bytesLen()) { return false; } // if the circular buffer doesnt contain N bytes
        // if data can be taken at once (i.e. in the middle)
        if (_storage_size > (_take_index+N)) {
          std::memcpy(dest.data(), _ptr+_take_index, N);
          _take_index += N; // update take_index
          _b_stored -= N; // update the number of stored bytes
          return true;
        }
        // if data must be copy in 2 different sections
        size_t num_a = _storage_size-_take_index; // number of bytes in first chunk
        std::memcpy(dest.data(), _ptr+_take_index, num_a); // first chunk
        std::memcpy(dest.data()+num_a, _ptr, N-num_a); // wrapped around chunk
        _take_index = N-num_a; // update take index
        _b_stored -= N; // update number of stored bytes
        return true;
      }

      // delete at most N bytes from buffer
      inline void drop(size_t N) {
        N = std::min(N, bytesLen()); // take minimum between N desired bytes to delete and the
                                     // number of stored bytes
        // instead of actually deleting the data
        // this function just re-index the buffer (faster, easier, and more efficient)
        if (_storage_size > (_take_index+N)) {
          _take_index += N; // re-indexing take index
          _b_stored -= N; // update number of stored bytes
        } else {
          _take_index = N-(_storage_size-_take_index);
          _b_stored -= N;
        }
      }

      // clear ring buffer (by re-indexing to 0)
      inline void clear() { _take_index = _b_stored = 0; }

      // resize circular buffer
      inline void resize(size_t N) {
        if (_storage_size == N) { return; } // size is already N so do nothing
        _take_index = _b_stored = 0;
        RawBuffer::operator =(RawBuffer(N));
      }

    protected:
      // current read pointer
      size_t _take_index;

      // offset of the write pointer relative to ptr
      size_t _b_stored;
  };

  // A Typed Circular Buffer
  template <class Scalar>
  class CircularBuffer: public RawCircularBuffer {
    public:
      // Empty constructor
      CircularBuffer() : RawCircularBuffer(), _size(0), _stored(0) {}

      // Construct from size N
      CircularBuffer(size_t N) : RawCircularBuffer(N*sizeof(Scalar)), _size(N), _stored(0) {}

      // Construct from another buffer
      CircularBuffer(const CircularBuffer<Scalar> &other)
        : RawCircularBuffer(other), _size(other._size), _stored(other._stored) {}

      // virtual destructor
      virtual ~CircularBuffer() {}
      
      // Operator Overloading
      // Assignment operator
      const CircularBuffer<Scalar> &operator = (const CircularBuffer<Scalar> &other) {
        RawCircularBuffer::operator =(other);
        _size = other._size;
        _stored = other._other;
        return *this;
      }

      // Indexing
      Scalar &operator[] (int index) {
        return reinterpret_cast<Scalar &> (RawCircularBuffer::operator [] (index*sizeof(Scalar)));
      }

      // Inline helper functions
      // return number of stored elements
      inline size_t stored() const { return _stored; }
      // return number of free elements
      inline size_t free() const { return _size-_stored; }
      // returns the size of ring buffer
      inline size_t size() const { return _size; }

      // Push and Pull functions
      // push function
      inline bool push(const Buffer<Scalar> &data) {
        if (RawCircularBuffer::push(data)) { // referencing push functions in original class
          _stored += data.size(); // update number of stored data
          return true;
        }
        return false; // return false if cant push data (i.e. sizes doesnt work out)
      }

      // pull function
      inline bool pull(const Buffer<Scalar> &dest, size_t N) {
        if (RawCircularBuffer::pull(dest, N*sizeof(Scalar))) {
          _stored -= N;
          return true;
        }
        return false;
      }

      // delete N elements from buffer
      inline void drop(size_t N) {
        RawCircularBuffer::drop(N*sizeof(Scalar));
        _stored = _b_stored/sizeof(Scalar);
      }

      // resize buffer to size N
      inline void resize(size_t N) {
        RawCircularBuffer::resize(N*sizeof(Scalar));
      }

    protected:
      // size of the circular buffer
      size_t _size;
      // number of stored entries
      size_t _stored;

  };

  // TODO: print pretty
}



#endif
