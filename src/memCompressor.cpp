#include "memCompressor.hpp"
#include <memory>

using namespace std;

class OutOfBoundsException : public exception {
    /* error message */
    virtual const char* what(void) const throw() { return "Index out of bounds"; }
};

/*** Compressable ***/

Compressable::Compressable(void) {}
// setters
void Compressable::id(unsigned int id) { this->id_ = id; }
void Compressable::data(float* data) { this->data_ = data; }

const float Compressable::read(unsigned int i) const {
    // check if i is in range
    if (i >= this->get_size()) throw OutOfBoundsException();
    // read and return value at index
    return this->data_[i];
}

void Compressable::write(unsigned int i, float v) {
    // check if i is in range
    if (i >= this->get_size()) throw OutOfBoundsException();
    // write new value at index
    this->data_[i] = v;
}


/*** Memory Compressor ***/

MemCompressor::MemCompressor(unsigned int mem_size): memory_size_(mem_size), filled_(0) {
    // allocate memory
    this->memory_ = new float[this->memory_size_];
    // create vectors
    this->instances_ = new vector<Compressable*>();
    this->type_ids_ = new vector<unsigned int>();
    // set memory tail
    this->memory_tail_ = this->memory_;
}

MemCompressor::~MemCompressor(void) {
    // free memory
    delete[] this->memory_;
    // delete all instances
    for (Compressable* e : *this->instances_) { delete e; }
    // delete vectors
    delete this->instances_;
    delete this->type_ids_;
}

