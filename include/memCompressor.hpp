#pragma once
#include <vector>
#include <exception>

class Config {};

class Compressable {
    private:
    /* reference to list to store values */
    float *data_;
    /* id of instance in memory compressor */
    unsigned int id_;

    protected:
    /* read-write data */
    const float read(unsigned int i) const;
    void write(unsigned int i, float v);

    public:
    /* constructors */
    Compressable(void);
    /* getters */
    unsigned int id(void) const { return this->id_; };
    /* setters */
    void id(unsigned int id);
    void data(float* data);
    /* get required data size to store instance */
    virtual unsigned int get_size(void) const = 0;
    /* get id of type */
    virtual unsigned int get_type_id(void) const = 0;
    /* apply config to obj */
    virtual void apply(Config* config) = 0;
};


/* Memory Compressor */

class MemoryOverflow : public std::exception {
    /* error message */
    virtual const char* what(void) const throw() { return "Memory Overflow in Memory Copressor."; }
};

class MemCompressor {

    private:
    /* memory */
    float* memory_; 
    float* memory_tail_;
    unsigned int memory_size_, filled_;
    /* store all instances */
    std::vector<Compressable*>* instances_; 
    std::vector<unsigned int>* type_ids_;

    public:
    /* constructors and destructor */
    MemCompressor(unsigned int mem_size);
    ~MemCompressor(void);
    /* getter */
    float* data(void) const { return this->memory_; }
    unsigned int filled(void) const { return this->filled_; }
    Compressable* get(unsigned int id) const { return this->instances_->at(id); }
    std::vector<Compressable*>* get_instances(void) const { return this->instances_; }
    std::vector<unsigned int>* get_type_ids(void) const { return this->type_ids_; }
    unsigned int n_instances(void) const { return this->instances_->size(); }
    /* factory method */
    template<class T> T* make(void) {
        // TODO: force T to inherit from Compressable
        // create instance of type
        Compressable *obj = new T();
        // check for space to store instance
        if (this->filled_ + obj->get_size() <= this->memory_size_) {
            // set up compressable
            obj->id(this->instances_->size());
            obj->data(this->memory_tail_);
            // update memory-tail and filled index
            this->memory_tail_ += obj->get_size();
            this->filled_ += obj->get_size();
            // add instance to vector
            this->instances_->push_back(obj);
            this->type_ids_->push_back(obj->get_type_id());
        // handle memory overflow
        } else throw MemoryOverflow();
        // return object
        return (T*)obj;
    }
    template<class T> T* make(Config* config) {
        // create instance
        Compressable* obj = this->make<T>();
        // apply config and delete it afterwards
        obj->apply(config);
        delete config;
        // return object
        return (T*)obj;
    }
};