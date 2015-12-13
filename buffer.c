#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "buffer.h"


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
    size_t len = (size_t) ftell(fd);
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
                    size_t pivot = buf->size / 16 * 16 + j;
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
    memcpy((char*) buf->bytes + buf->pivot, bytes, length);
    buf->pivot += length;
}

void buf_readbytes(buffer_t* buf, void* dest, const size_t length) {
    memcpy(dest, (char*) buf->bytes + buf->pivot, length);
    buf->pivot += length;
}

char* buf_readstr(buffer_t* buf) {
    size_t len = strlen((char*) buf->bytes + buf->pivot);
    char* ret = malloc(len * sizeof(char) + 1);
    if (ret == NULL) exit(errno);
    ret[0] = '\0';
    strncat(ret, (char*) buf->bytes + buf->pivot, len);
    buf->pivot += len + 1;
    return ret;
}

void buf_free(buffer_t* buf) {
    free(buf->bytes);
    buf->bytes = NULL;
    free(buf);
    buf = NULL;
}
