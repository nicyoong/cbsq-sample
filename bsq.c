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

int parse_map(char **lines, size_t num_lines_content, int *parsed_num_lines, int *parsed_map_width, char *empty, char *obstacle, char *full, char ***map_data) {
    if (num_lines_content < 2) return -1;

    char *first_line = lines[0];
    size_t first_len = strlen(first_line);
    if (first_len < 4) return -1;

    char *num_str = malloc(first_len - 3 + 1);
    if (!num_str) return -1;
    memcpy(num_str, first_line, first_len - 3);
    num_str[first_len - 3] = '\0';

    for (char *p = num_str; *p; p++) {
        if (*p < '0' || *p > '9') {
            free(num_str);
            return -1;
        }
    }

    char *endptr;
    long num = strtol(num_str, &endptr, 10);
    free(num_str);
    if (endptr == num_str || *endptr != '\0' || num <= 0 || num > INT_MAX) return -1;

    *parsed_num_lines = num;
    *empty = first_line[first_len - 3];
    *obstacle = first_line[first_len - 2];
    *full = first_line[first_len - 1];

    if (*empty == *obstacle || *empty == *full || *obstacle == *full) return -1;

    size_t remaining_lines = num_lines_content - 1;
    if (remaining_lines != (size_t)*parsed_num_lines) return -1;

    if (remaining_lines == 0) return -1;

    *map_data = &lines[1];
    int map_width = strlen((*map_data)[0]);
    if (map_width == 0) return -1;

    for (size_t i = 0; i < remaining_lines; i++) {
        if (strlen((*map_data)[i]) != map_width) return -1;
        for (int j = 0; j < map_width; j++) {
            char c = (*map_data)[i][j];
            if (c != *empty && c != *obstacle) return -1;
        }
    }

    *parsed_map_width = map_width;
    return 0;
}

void find_largest_square(int num_lines, int map_width, char **map_data, char empty, char full) {
    int **dp = malloc(num_lines * sizeof(int *));
    for (int i = 0; i < num_lines; i++) {
        dp[i] = malloc(map_width * sizeof(int));
        for (int j = 0; j < map_width; j++) {
            dp[i][j] = 0;
        }
    }

    int max_size = 0;
    int bi = 0, bj = 0;

    for (int i = 0; i < num_lines; i++) {
        for (int j = 0; j < map_width; j++) {
            if (map_data[i][j] == empty) {
                if (i == 0 || j == 0) {
                    dp[i][j] = 1;
                } else {
                    int min = dp[i-1][j];
                    if (dp[i][j-1] < min) min = dp[i][j-1];
                    if (dp[i-1][j-1] < min) min = dp[i-1][j-1];
                    dp[i][j] = min + 1;
                }
                if (dp[i][j] > max_size) {
                    max_size = dp[i][j];
                    bi = i;
                    bj = j;
                }
            }
        }
    }

    if (max_size > 0) {
        int start_row = bi - max_size + 1;
        int start_col = bj - max_size + 1;
        for (int i = start_row; i <= bi; i++) {
            for (int j = start_col; j <= bj; j++) {
                map_data[i][j] = full;
            }
        }
    }

    for (int i = 0; i < num_lines; i++) {
        free(dp[i]);
    }
    free(dp);
}

void print_map(char **map_data, int num_lines) {
    if (num_lines == 0) return;
    printf("%s", map_data[0]);
    for (int i = 1; i < num_lines; i++) {
        printf("\n%s", map_data[i]);
    }
    printf("\n");
}

void process_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "map error 1\n");
        return;
    }

    size_t content_len;
    char *content = read_file_content(file, &content_len);
    fclose(file);
    if (!content) {
        fprintf(stderr, "map error 2\n");
        return;
    }

    size_t num_lines_content;
    char **lines = split_into_lines(content, &num_lines_content);
    if (!lines) {
        free(content);
        fprintf(stderr, "map error 3\n");
        return;
    }

    int parsed_num_lines, parsed_map_width;
    char empty, obstacle, full;
    char **map_data;

    if (parse_map(lines, num_lines_content, &parsed_num_lines, &parsed_map_width, &empty, &obstacle, &full, &map_data) != 0) {
        free(lines);
        free(content);
        fprintf(stderr, "map error 4\n");
        return;
    }

    find_largest_square(parsed_num_lines, parsed_map_width, map_data, empty, full);
    print_map(map_data, parsed_num_lines);

    free(lines);
    free(content);
}