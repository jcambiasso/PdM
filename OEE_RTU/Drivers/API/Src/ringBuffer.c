#include "ringBuffer.h"

#ifdef __cplusplus
extern "C" 
{
#endif
typedef enum arrayOperations_e{
    INCREMENT,
    DECREMENT,
    OPERATIONS
} arrayOperations_t;

/**
 * @brief Returns memory position in bytes as offset of basePointer of next free array position.
 * 
 * @param rb Target ringBuffer
 * @return pointer_t offset in bytes
 */
static pointer_t searchEmpty(RingBuffer_t* rb){
    rb->full = true;
    for(pointer_t i = 0; i<rb->length; i++){
        pointer_t pos = i*rb->dataSize;
        if(((uint8_t*)rb->basePointer)[pos] == 0){
            rb->full = false;
            return pos;
        }
    }
    return 0;
}

static pointer_t safeArrayOperation(arrayOperations_t operation, pointer_t data, pointer_t floor, pointer_t top, size_t size){
    switch (operation)
    {
    case INCREMENT:
    {
        data+= size;
        if(data>=top) data = floor;
    }
    break;
    case DECREMENT:
    {
        data-= size;
        if(data<=floor) data = top;
    }
    break;
    default: return 0;
        break;
    }
    return data;       
}

bool rb_init(RingBuffer_t *rb, const void * arr, size_t dataSize, size_t arrayElements){
    rb->basePointer = arr;
    rb->dataSize = dataSize;
    rb->length = arrayElements;
    rb->readPointer = 0;
    rb->writePointer = 0;
    rb->count = 0;
    for(pointer_t i = 0; i<arrayElements;i++)
    {
        //Invalidate all data in the array.
        ((uint8_t*)rb->basePointer)[i*rb->dataSize] = 0;
    }
    return true;
}

bool list_init(RingBuffer_t *rb, const void * arr, size_t dataSize, size_t arrayElements){
    rb_init(rb,arr,dataSize,arrayElements);
    rb->asList = true;
    return true;
}

bool rb_pop(RingBuffer_t *rb, void* destination){
    if(rb->asList) return false;
    if(rb->full) rb->full = false;
    void* pointer = (void*)((pointer_t)rb->basePointer + rb->readPointer);
    //Avoids copying memory if its invalid data.
    if(((uint8_t*)pointer)[0] == 0) return false;
    memcpy(destination, pointer, rb->dataSize);
    rb->count--;
    ((uint8_t*)pointer)[0] = 0;
    pointer_t top = rb->dataSize*rb->length;
    rb->readPointer = safeArrayOperation(INCREMENT,rb->readPointer, 0, top, rb->dataSize);
    return true;
}

bool rb_push(RingBuffer_t *rb, void* data){
    void* pointer;
    if(rb->asList){
        pointer = (void*)((pointer_t)rb->basePointer + searchEmpty(rb));
        if(rb->full) return false;
        else{
            memcpy(pointer, data, rb->dataSize);
            rb->count++;
            return true;
        }
    }
    pointer = (void*)((pointer_t)rb->basePointer + rb->writePointer);
    if(((uint8_t*)pointer)[0] == 1) 
    {
        rb->full = true;
        return false;
    }
    else{
        memcpy(pointer, data, rb->dataSize);
        rb->count++;
        pointer_t top = rb->dataSize*rb->length;
        rb->writePointer = safeArrayOperation(INCREMENT,rb->writePointer, 0, top, rb->dataSize);
        ((uint8_t*)pointer)[0] = 1;
        return true;
    }
}

bool rb_peek(RingBuffer_t *rb, void* destination){
    void* pointer = (void*)((pointer_t)rb->basePointer + rb->readPointer);
    //Avoids copying memory if its invalid data.
    if(((uint8_t*)pointer)[0] == 0) return false;
    memcpy(destination, pointer, rb->dataSize);
    return true;
}

pointer_t rb_count(RingBuffer_t *rb){
    if(!rb->asList) return rb->count;
    pointer_t count = 0;
    for(pointer_t i = 0; i<rb->length;i++)
    {
        if(((uint8_t*)rb->basePointer)[i*rb->dataSize] == 1) count ++;
    }
    return count;
}

bool rb_flush(RingBuffer_t *rb){
    rb_init(rb, rb->basePointer, rb->dataSize, rb->length);
    return true;
}

void* rb_getHead(RingBuffer_t *rb){
    return (void*)((pointer_t)rb->basePointer + rb->readPointer);
}

bool list_push(RingBuffer_t *rb, void* data){
    rb->asList = true;
    return rb_push(rb,data);
}

bool list_pushPosition(RingBuffer_t *rb, void* data, pointer_t position){
    rb->asList = true;
    if(position > rb->length) return false;
    void* pointer = (void*)((pointer_t)rb->basePointer + rb->dataSize*position);
    memcpy(pointer, data, rb->dataSize);
    rb->count++;
    return true;
}

bool list_popPosition(RingBuffer_t *rb, void* destination, pointer_t position){
    rb->asList = true;
    if(position > rb->length) return false;
    void* pointer = (void*)((pointer_t)rb->basePointer + rb->dataSize*position);
    memcpy(destination, pointer, rb->dataSize);
    ((uint8_t*)pointer)[0] = 0;
    rb->count--;
    return true;
}

#ifdef __cplusplus
}

RingBuffer::RingBuffer(const void * arr, size_t dataSize, size_t arrayElements){
    rb_init(this, arr, dataSize, arrayElements);
}

bool RingBuffer::pop(void* destination){
    return rb_pop(this, destination);
}
bool RingBuffer::push(void* data){
    return rb_push(this,data);
}

bool RingBuffer::push(void* data, pointer_t posistion){
    return list_pushPosition(this,data,posistion);
}

bool RingBuffer::peek(void* destination){
    return rb_peek(this,destination);
}
bool RingBuffer::flush(){
    return rb_flush(this);
}
long int RingBuffer::count(){
    return rb_count(this);
}

void* RingBuffer::getBuffer(){
    return (void*)this->basePointer;
}

void RingBuffer::toList(){
    this->asList = true;
}
#endif
