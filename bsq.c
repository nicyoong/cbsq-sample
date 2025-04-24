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

char **split_into_lines(char *buffer, size_t *num_lines) {
    if (!buffer) return NULL;

    char **lines = NULL;
    size_t capacity = 0;
    size_t count = 0;
    char *start = buffer;

    while (*start) {
        // Find the next line ending or end of string
        size_t span = strcspn(start, "\r\n");
        char *end = start + span;

        // Handle line endings
        char *next_line = end;
        if (*next_line) {
            // Null-terminate the current line
            *next_line = '\0';
            // Move past all \r and \n characters
            do {
                next_line++;
            } while (*next_line == '\r' || *next_line == '\n');
        }

        // Trim trailing \r from the current line
        size_t line_len = strlen(start);
        if (line_len > 0 && start[line_len - 1] == '\r') {
            start[line_len - 1] = '\0';
            line_len--;
        }

        // Skip adding empty line ONLY if it's at the very end of the buffer
        if (line_len == 0 && *next_line == '\0') {
            // This is a trailing empty line caused by a final newline; skip it
        } else {
            // Add the line to the array
            if (count >= capacity) {
                size_t new_capacity = capacity == 0 ? 16 : capacity * 2;
                char **new_lines = realloc(lines, new_capacity * sizeof(char *));
                if (!new_lines) {
                    free(lines);
                    return NULL;
                }
                lines = new_lines;
                capacity = new_capacity;
            }
            lines[count++] = start;
        }

        start = next_line;
    }

    *num_lines = count;
    return lines;
}