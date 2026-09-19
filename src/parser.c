#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"

static char *copy_string(const char *text)
{
    if (text == NULL) {
        return NULL;
    }

    char *copy = malloc(strlen(text) + 1);

    if (copy == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(copy, text);

    return copy;
}

void command_init(Command *cmd)
{
    cmd->argc = 0;
    cmd->capacity = 8;

    cmd->argv = malloc(sizeof(char *) * cmd->capacity);

    if (cmd->argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    cmd->argv[0] = NULL;

    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->error_file = NULL;

    cmd->append_output = 0;
    cmd->background = 0;
    cmd->pipe_after = 0;
}

static void add_argument(Command *cmd, const char *value)
{
    if (cmd->argc + 1 >= cmd->capacity) {

        cmd->capacity *= 2;

        char **new_argv =
            realloc(cmd->argv,
                    sizeof(char *) * cmd->capacity);

        if (new_argv == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        cmd->argv = new_argv;
    }

    cmd->argv[cmd->argc] = copy_string(value);

    cmd->argc++;

    cmd->argv[cmd->argc] = NULL;
}

void command_free(Command *cmd)
{
    if (cmd == NULL) {
        return;
    }

    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }

    free(cmd->argv);

    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd->error_file);

    cmd->argv = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->error_file = NULL;

    cmd->argc = 0;
    cmd->capacity = 0;
}

static void add_command(CommandList *list, Command *cmd)
{
    if (list->count >= list->capacity) {

        list->capacity *= 2;

        Command *new_commands =
            realloc(list->commands,
                    sizeof(Command) * list->capacity);

        if (new_commands == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        list->commands = new_commands;
    }

    list->commands[list->count] = *cmd;

    list->count++;

    command_init(cmd);
}

int parse_tokens(token_list_t *tokens, CommandList *list)
{
    if (tokens == NULL || list == NULL) {
        return -1;
    }

    list->count = 0;
    list->capacity = 4;

    list->commands =
        malloc(sizeof(Command) * list->capacity);

    if (list->commands == NULL) {
        perror("malloc");
        return -1;
    }

    Command current;

    command_init(&current);

    for (size_t i = 0; i < tokens->count; i++) {

        Token *token = &tokens->tokens[i];

        switch (token->type) {

        case TOKEN_WORD:

            add_argument(&current, token->value);
            break;

        case TOKEN_INPUT:

            if (i + 1 >= tokens->count ||
                tokens->tokens[i + 1].type != TOKEN_WORD) {

                fprintf(stderr,
                        "shellforge: expected input file\n");

                command_free(&current);
                command_list_free(list);

                return -1;
            }

            free(current.input_file);

            current.input_file =
                copy_string(tokens->tokens[++i].value);

            break;

        case TOKEN_OUTPUT:

            if (i + 1 >= tokens->count ||
                tokens->tokens[i + 1].type != TOKEN_WORD) {

                fprintf(stderr,
                        "shellforge: expected output file\n");

                command_free(&current);
                command_list_free(list);

                return -1;
            }

            free(current.output_file);

            current.output_file =
                copy_string(tokens->tokens[++i].value);

            current.append_output = 0;

            break;

        case TOKEN_APPEND:

            if (i + 1 >= tokens->count ||
                tokens->tokens[i + 1].type != TOKEN_WORD) {

                fprintf(stderr,
                        "shellforge: expected output file\n");

                command_free(&current);
                command_list_free(list);

                return -1;
            }

            free(current.output_file);

            current.output_file =
                copy_string(tokens->tokens[++i].value);

            current.append_output = 1;

            break;

        case TOKEN_ERROR:
            if (i + 1 >= tokens->count ||
                tokens->tokens[i + 1].type != TOKEN_WORD) {
                fprintf(stderr, "shellforge: expected error file\n");
                command_free(&current);
                command_list_free(list);
                return -1;
            }
            free(current.error_file);
            current.error_file = copy_string(tokens->tokens[++i].value);
            break;

        case TOKEN_BACKGROUND:

            current.background = 1;
            break;

        case TOKEN_PIPE:

            if (current.argc == 0) {

                fprintf(stderr,
                        "shellforge: invalid pipe\n");

                command_free(&current);
                command_list_free(list);

                return -1;
            }

            current.pipe_after = 1;
            add_command(list, &current);
            break;

        case TOKEN_END:

            if (current.argc > 0) {
                add_command(list, &current);
            }

            break;
        }
    }

    if (current.argc > 0) {
        add_command(list, &current);
    }

    command_free(&current);

    return 0;
}

void command_list_free(CommandList *list)
{
    if (list == NULL) {
        return;
    }

    for (int i = 0; i < list->count; i++) {
        command_free(&list->commands[i]);
    }

    free(list->commands);

    list->commands = NULL;
    list->count = 0;
    list->capacity = 0;
}
