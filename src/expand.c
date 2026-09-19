#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/expand.h"

static void append_string(char **result,
                          size_t *length,
                          size_t *capacity,
                          const char *text)
{
    size_t text_length = strlen(text);

    while (*length + text_length + 1 > *capacity) {

        *capacity *= 2;

        char *new_result =
            realloc(*result, *capacity);

        if (new_result == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        *result = new_result;
    }

    memcpy(*result + *length,
           text,
           text_length);

    *length += text_length;

    (*result)[*length] = '\0';
}

char *expand_variable(const char *input)
{
    if (input == NULL) {
        return NULL;
    }

    size_t capacity = 128;
    size_t length = 0;

    char *result = malloc(capacity);

    if (result == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    result[0] = '\0';

    for (size_t i = 0;
         input[i] != '\0';) {

        if (input[i] == '$') {

            i++;

            char variable[256];
            size_t j = 0;

            if (input[i] == '{') {

                i++;

                while (input[i] != '\0' &&
                       input[i] != '}' &&
                       j < sizeof(variable) - 1) {

                    variable[j++] = input[i++];
                }

                if (input[i] == '}') {
                    i++;
                }

            } else {

                while (input[i] != '\0' &&
                       (isalnum((unsigned char)input[i]) ||
                        input[i] == '_') &&
                       j < sizeof(variable) - 1) {

                    variable[j++] = input[i++];
                }
            }

            variable[j] = '\0';

            if (j > 0) {

                const char *value =
                    getenv(variable);

                if (value != NULL) {

                    append_string(&result,
                                  &length,
                                  &capacity,
                                  value);
                }

                continue;
            }

            append_string(&result,
                          &length,
                          &capacity,
                          "$");

        } else if (input[i] == '~' &&
                   (i == 0 || input[i - 1] == '/')) {

            const char *home = getenv("HOME");

            if (home != NULL) {

                append_string(&result,
                              &length,
                              &capacity,
                              home);
            }

            i++;

        } else {

            char temp[2];

            temp[0] = input[i];
            temp[1] = '\0';

            append_string(&result,
                          &length,
                          &capacity,
                          temp);

            i++;
        }
    }

    return result;
}

void expand_command(Command *cmd)
{
    if (cmd == NULL) {
        return;
    }

    for (int i = 0; i < cmd->argc; i++) {

        char *expanded =
            expand_variable(cmd->argv[i]);

        free(cmd->argv[i]);

        cmd->argv[i] = expanded;
    }

    if (cmd->input_file != NULL) {

        char *expanded =
            expand_variable(cmd->input_file);

        free(cmd->input_file);

        cmd->input_file = expanded;
    }

    if (cmd->output_file != NULL) {

        char *expanded =
            expand_variable(cmd->output_file);

        free(cmd->output_file);

        cmd->output_file = expanded;
    }

    if (cmd->error_file != NULL) {

        char *expanded =
            expand_variable(cmd->error_file);

        free(cmd->error_file);

        cmd->error_file = expanded;
    }
}
