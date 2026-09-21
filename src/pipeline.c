#include <stdio.h>

#include "pipeline.h"

int pipeline_build(const token_list_t *tokens, pipeline_t *pipeline)
{
    if (tokens == NULL || pipeline == NULL)
        return -1;

    pipeline->command_count = 0;

    int command_index = 0;
    int argument_index = 0;

    for (int i = 0; i < tokens->count; i++)
    {
        const token_t *token = &tokens->tokens[i];

        /*
         * End of token list.
         */
        if (token->type == TOKEN_END)
            break;

        /*
         * Pipe separates two commands.
         */
        if (token->type == TOKEN_PIPE)
        {
            /*
             * Reject empty command:
             * ls | | wc
             */
            if (argument_index == 0)
            {
                fprintf(stderr, "shellforge: invalid null command\n");
                return -1;
            }

            /*
             * Terminate current command's argv.
             */
            pipeline->commands[command_index][argument_index] = NULL;
            pipeline->argc[command_index] = argument_index;

            command_index++;
            argument_index = 0;

            /*
             * Too many commands.
             */
            if (command_index >= MAX_COMMANDS)
            {
                fprintf(stderr, "shellforge: too many commands\n");
                return -1;
            }

            continue;
        }

        /*
         * Milestone 4.1 currently supports
         * only WORD tokens inside pipelines.
         */
        if (token->type != TOKEN_WORD)
        {
            fprintf(stderr,
                    "shellforge: unsupported token in pipeline\n");
            return -1;
        }

        /*
         * Leave one slot for NULL terminator.
         */
        if (argument_index >= MAX_TOKENS - 1)
        {
            fprintf(stderr, "shellforge: too many arguments\n");
            return -1;
        }

        /*
         * Store pointer to the token text.
         */
        pipeline->commands[command_index][argument_index] =
            token->text;

        argument_index++;
    }

    /*
     * The final command must not be empty.
     *
     * Reject:
     * ls |
     */
    if (argument_index == 0)
    {
        fprintf(stderr, "shellforge: invalid null command\n");
        return -1;
    }

    /*
     * Terminate final command's argv.
     */
    pipeline->commands[command_index][argument_index] = NULL;
    pipeline->argc[command_index] = argument_index;

    /*
     * Number of commands = last index + 1.
     */
    pipeline->command_count = command_index + 1;

    return 0;
}

