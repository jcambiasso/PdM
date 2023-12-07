/**
 * @file ringBuffer.h
 * @author Javier Cambiasso
 * @brief FIFO Ring buffer for static memory
 * @version 0.1
 * @date 2023-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

//Change according to system
#define POINTER_SIZE uint32_t

#ifdef __cplusplus
extern "C"{
#endif

typedef POINTER_SIZE pointer_t;

typedef struct RingBuffer_s{
    //All pointers and except location are in data chunks, not in bytes. Actual memory location is pointer*dataSize.
    //All pointers are dataSize offsets of location, up to basePointer+65536*dataSize in memory depth.
    unsigned int length;
    unsigned int dataSize;
    const void* basePointer; 
    pointer_t writePointer;
    pointer_t readPointer;
    pointer_t count;
    bool full;
    bool asList;
} RingBuffer_t;

/**
 * @brief Initializes a ring buffer.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param array Pointer to the start of the data array. First byte must be free of data (use a boolean or something)!
 * @param dataSize size of data in bytes (eg output of sizeof(struct))
 * @param arrayElements Total amount of elements in the array (output of sizeof(array)/sizeof(struct))
 */
bool rb_init(RingBuffer_t *rb, const void * array, size_t dataSize, size_t arrayElements);
/**
 * @brief Initializes a ring buffer as a list.
 * 
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param array Pointer to the start of the data array. First byte must be free of data (use a boolean or something)!
 * @param dataSize size of data in bytes (eg output of sizeof(struct))
 * @param arrayElements Total amount of elements in the array (output of sizeof(array)/sizeof(struct))
 */
bool list_init(RingBuffer_t *rb, const void * array, size_t dataSize, size_t arrayElements);
/**
 * @brief Reads a data chunk from the buffer
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param destination Pointer to data structure you want to read into
 * @return true If success, returns true.
 * @return false If the buffer is empty or is operating as list, returns false.
 */
bool rb_pop(RingBuffer_t *rb, void* destination);
/**
 * @brief Stores data in buffer.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param data Pointer to source data
 * @return true Returns true if success
 * @return false If the buffer is full, returns false.
 */
bool rb_push(RingBuffer_t *rb, void* data);
/**
 * @brief Reads a data chunk without advancing the buffer
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param destination Pointer to data structure you want to read into
 * @return true Return true if valid data is in head.
 * @return false Returns false if the buffer is empty.
 */
bool rb_peek(RingBuffer_t *rb, void* destination);
/**
 * @brief Clears buffer (just mark for replacement, data is there.)
 * 
 * @param rb Pointer to the ring buffer struct.
 * @return true Always returns true.
 * @return false 
 */
bool rb_flush(RingBuffer_t *rb);
/**
 * @brief Counts elements with unread data.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @return pointer_t Number of elements with unread data.
 */
pointer_t rb_count(RingBuffer_t *rb);
/**
 * @brief Gets the heading data pointer to edit data directly.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @return void* Pointer to the head data stored in the main array.
 */
void* rb_getHead(RingBuffer_t *rb);
/**
 * @brief Push data to the next available slot in the buffer. Note that using disables FIFO operation.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param data Pointer to source data
 * @return true Returns true if success
 * @return false If the buffer is full, returns false.
 */
bool list_push(RingBuffer_t *rb, void* data);
/**
 * @brief Same as push but to an specific position, overwriting data.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param data Pointer to source data
 * @return true Returns true if success
 * @return false If position exceeds array size, returns false.
 */
bool list_pushPosition(RingBuffer_t *rb, void* data, pointer_t position);

/**
 * @brief Same as pop but to an specific position.
 * 
 * @param rb Pointer to the ring buffer struct.
 * @param data Pointer to destination data
 * @return true Returns true if success
 * @return false If position exceeds array size, returns false.
 */
bool list_popPosition(RingBuffer_t *rb, void* destination, pointer_t position);
#ifdef __cplusplus
}

class RingBuffer: private RingBuffer_t{
    public:
        RingBuffer(const void * array, size_t dataSize, size_t arrayElements);
        operator ::RingBuffer_t*();
	    operator const ::RingBuffer_t*();
        bool pop(void* destination);
        bool push(void* data);
        bool push(void* data, pointer_t position);
        bool peek(void* destination);
        bool flush();
        long int count();
        void* getBuffer();
        void toList();
    private:

    protected:

};

#endif

#endif //RINGBUFFER_H
