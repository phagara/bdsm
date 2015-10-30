#ifndef BUFFER_H
#define BUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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


/*
 * implementation
 */

buffer_t* buf_init(void) {
    buffer_t* buf = malloc(sizeof(buffer_t));
    if (buf == NULL) exit(errno);

    buf->pivot = 0;
    buf->size = 0;
    buf->bytes = malloc(0);
    if (buf->bytes == NULL) exit(errno);

    return buf;
}

buffer_t* buf_init_from_fd(FILE* fd) {
    fseek(fd, 0, SEEK_END);
    size_t len = ftell(fd);
    rewind(fd);

    buffer_t* buf = malloc(sizeof(buffer_t));
    if (buf == NULL) exit(errno);

    buf->pivot = 0;
    buf->size = len;
    buf->bytes = malloc(len);
    if (buf->bytes == NULL) exit(errno);

    fread(buf->bytes, buf->size, 1, fd);

    return buf;
}

void buf_print(buffer_t* buf) {
    for (size_t i=0; i<buf->size; i++) {
        printf("%02x", ((unsigned char*) buf->bytes)[i]);

        if ((i+1) % 16 == 0) {
            printf("\t[");
            for (unsigned int j=16; j>0; j--) {
                if (isprint(((unsigned char*)buf->bytes)[i-j+1]))
                    printf("%c", ((unsigned char*) buf->bytes)[i-j+1]);
                else printf(".");
            }
            printf("]\n");
        }
        else if ((i+1) % 8 == 0)
            printf("  ");
        else
            printf(" ");
    }

    if (buf->size % 16 != 0 && buf->size) {
        unsigned int num_empty_bytes = 16 - (buf->size % 16);
        for (unsigned int i=0; i<num_empty_bytes; i++) {
            printf("  ");
            if ((i+1) % 16 == num_empty_bytes % 16) {
                printf("\t[");
                for (unsigned int j=0; j<16-num_empty_bytes; j++) {
                    unsigned int pivot = buf->size / 16 * 16 + j;
                    if (isprint(((unsigned char*)buf->bytes)[pivot]))
                        printf("%c", ((unsigned char*)buf->bytes)[pivot]);
                    else
                        printf(".");
                }
                for (unsigned int j=0; j<num_empty_bytes; j++)
                    printf(" ");
                printf("]\n");
            }
            else if ((i+1) % 8 == num_empty_bytes % 8)
                printf("  ");
            else
                printf(" ");
        }
    }
}

void buf_rewind(buffer_t* buf) {
    buf->pivot = 0;
}

void buf_extend(buffer_t* buf, const size_t size) {
    buf->size += size;
    buf->bytes = realloc(buf->bytes, buf->size);
    if (buf->bytes == NULL) exit(errno);
}

void buf_write(buffer_t* buf, const void* bytes, const size_t length) {
    if (buf->size - buf->pivot < length)
        buf_extend(buf, length - (buf->size - buf->pivot));
    memcpy(buf->bytes + buf->pivot, bytes, length);
    buf->pivot += length;
}

void buf_readbytes(buffer_t* buf, void* dest, const size_t length) {
    memcpy(dest, buf->bytes + buf->pivot, length);
    buf->pivot += length;
}

char* buf_readstr(buffer_t* buf) {
    size_t len = strlen(buf->bytes + buf->pivot);
    char* ret = malloc(len * sizeof(char) + 1);
    if (ret == NULL) exit(errno);
    ret[0] = '\0';
    strncat(ret, buf->bytes + buf->pivot, len);
    buf->pivot += len + 1;
    return ret;
}

void buf_free(buffer_t* buf) {
    free(buf->bytes);
    buf->bytes = NULL;
    free(buf);
    buf = NULL;
}
#endif
