#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <stdio.h>
#include <string.h>
#include <ctype.h>


/*
 * structs
 */

typedef struct buffer_struct {
    size_t pivot;
    size_t size;
    void* bytes;
} buffer_t;


/*
 * function prototypes
 */

// allocates a new empty buffer
buffer_t* buf_init(void);

// creates a new buffer and reads data from an open file into it
buffer_t* buf_init_from_fd(FILE* fd);

// prints a hex dump of the buffer
void buf_print(buffer_t* buf);

// resets the buffer pivot back to the beginning
void buf_rewind(buffer_t* buf);

// increases buffer length by specified size
void buf_extend(buffer_t* buf, const size_t size);

// writes length bytes into the buffer,
// starting from buf->bytes[buf->pivot]
void buf_write(buffer_t* buf, const void* bytes, const size_t length);

// reads length bytes from the buffer into dest,
// starting from buf->bytes[buf->pivot]
void buf_readbytes(buffer_t* buf, void* dest, const size_t length);

// reads (and allocates memory for!) a null-terminated string
// from the buffer, starting from buf->bytes[buf->pivot]
char* buf_readstr(buffer_t* buf);

// deallocates the buffer
void buf_free(buffer_t* buf);

#endif
