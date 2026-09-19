#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "token.h"
#include "lexer.h"
#include "history.h"

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

        if (strcmp(line, "exit") == 0)
        {
            free(line);
            break;
        }

        if (strcmp(line, "history") == 0)
        {
            history_print();
            free(line);
            continue;
        }

        token_list_t tokens;

        if (lexer(line, &tokens) == 0)
        {
            token_print(&tokens);
        }

        free(line);
    }

    history_free();

    return 0;
}
