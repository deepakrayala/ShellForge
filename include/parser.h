#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct {
    char **argv;
    int argc;
    int capacity;

    char *input_file;
    char *output_file;
    char *error_file;

    int append_output;
    int background;
    int pipe_after;
} Command;

typedef struct {
    Command *commands;
    int count;
    int capacity;
} CommandList;

void command_init(Command *cmd);
void command_free(Command *cmd);

int parse_tokens(token_list_t *tokens, CommandList *list);

void command_list_free(CommandList *list);

#endif
