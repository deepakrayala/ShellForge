#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "lexer.h"
#include "history.h"
#include "builtin.h"
#include "executor.h"
int main(void)
{
    printf("=====================================\n");
    printf("          Shellforge\n");
    printf(" A Unix Style Shell written in C\n");
    printf("=====================================\n");

    while (1)
    {
        char *line = readline("shellforge$ ");

        if (line == NULL)
        {
            printf("\nExiting...\n");
            break;
        }

        if (strlen(line) == 0)
        {
            free(line);
            continue;
        }

        add_history(line);
        history_add(line);

        token_list_t tokens;

        if (lexer(line, &tokens) != 0)
        {
            free(line);
            continue;
        }

        /*
         * Check for built-in commands
         */
        if (tokens.count > 0 &&
            tokens.tokens[0].type == TOKEN_WORD &&
            is_builtin(tokens.tokens[0].text))
        {
            /*
             * Create argument array for builtin
             */
            char *argv[MAX_TOKENS];
            int argc = 0;

            for (int i = 0; i < tokens.count; i++)
            {
                if (tokens.tokens[i].type == TOKEN_END)
                    break;

                if (tokens.tokens[i].type != TOKEN_WORD)
                    break;

                argv[argc++] = tokens.tokens[i].text;
            }

            argv[argc] = NULL;

            /*
             * Execute builtin
             */
            int result = execute_builtin(argv);

            /*
             * Builtin exit
             */
            if (strcmp(argv[0], "exit") == 0 && result == 1)
            {
                free(line);
                break;
            }

            free(line);
            continue;
        }

        /*
 * Execute external command
 */
char *argv[MAX_TOKENS];
int argc = 0;

for (int i = 0; i < tokens.count; i++)
{
    if (tokens.tokens[i].type == TOKEN_END)
        break;

    if (tokens.tokens[i].type != TOKEN_WORD)
        break;

    argv[argc++] = tokens.tokens[i].text;
}

argv[argc] = NULL;

if (argc > 0)
{
    execute_external(argv);
}

free(line);
    }

    history_free();

    return 0;
}
