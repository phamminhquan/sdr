#include "buffer.h"
#include <stdlib.h>

using namespace sdr;

// empty constructor
RawBuffer::RawBuffer()
  : _ptr(0), _storage_size(0), _offset(0), _length(0), _refcount(0) 
{}

// construct buffer of size N
RawBuffer::RawBuffer(size_t N)
  : _ptr((char*)malloc(N)), _storage_size(N), _offset(0), _length(N), _refcount((int*)malloc(sizeof(int)))
{
  if ((_ptr == 0) && (_refcount != 0)) {
    free(_refcount); // free this memory location
    _refcount = 0;
    _storage_size = 0;
    return;
  }
  
  if (_refcount) { (*_refcount) = 1; }
}

// construct from data
RawBuffer::RawBuffer(char* data, size_t offset, size_t len)
  : _ptr(data), _storage_size(offset+len), _offset(offset), _length(len), _refcount(0)
{}

// construct from another buffe
RawBuffer::RawBuffer(const RawBuffer &other)
  : _ptr(other._ptr), _storage_size(other._storage_size), _offset(other._offset), _length(other._length), _refcount(other._refcount)
{}

// Create new view on buffer
RawBuffer::RawBuffer(const RawBuffer &other, size_t offset, size_t len)
  : _ptr(other._ptr), _storage_size(other._storage_size), _offset(other._offset+offset), _length(len), _refcount(other._refcount)
{}

// virtual destructor
RawBuffer::~RawBuffer()
{}

// Circular Buffers
RawCircularBuffer::RawCircularBuffer()
  : RawBuffer(), _take_index(0), _b_stored(0) 
{}

RawCircularBuffer::RawCircularBuffer(size_t size)
  : RawBuffer(size), _take_index(0), _b_stored(0)
{}

RawCircularBuffer::RawCircularBuffer(const RawCircularBuffer &other)
  : RawBuffer(other), _take_index(other._take_index), _b_stored(other._b_stored)
{}

RawCircularBuffer::~RawCircularBuffer()
{}

