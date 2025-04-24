#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

char *read_file_content(FILE *file, size_t *len) {
    char *buffer = NULL;
    size_t buffer_size = 0;
    size_t content_size = 0;
    char chunk[4096];

    while (1) {
        size_t read_size = fread(chunk, 1, sizeof(chunk), file);
        if (read_size == 0) {
            if (ferror(file)) {
                free(buffer);
                return NULL;
            }
            break;
        }
        if (content_size + read_size + 1 > buffer_size) {
            size_t new_size = buffer_size + read_size + 4096;
            char *new_buffer = realloc(buffer, new_size);
            if (!new_buffer) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
            buffer_size = new_size;
        }
        memcpy(buffer + content_size, chunk, read_size);
        content_size += read_size;
    }
    if (buffer) {
        buffer[content_size] = '\0';
    }
    *len = content_size;
    return buffer;
}