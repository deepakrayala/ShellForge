#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "builtin.h"

int is_builtin(char *cmd)
{
    if (cmd == NULL)
        return 0;

    if (strcmp(cmd, "cd") == 0 ||
        strcmp(cmd, "pwd") == 0 ||
        strcmp(cmd, "echo") == 0 ||
        strcmp(cmd, "exit") == 0)
    {
        return 1;
    }

    return 0;
}

int execute_builtin(char **argv)
{
    if (argv == NULL || argv[0] == NULL)
        return 0;

    /* cd */
    if (strcmp(argv[0], "cd") == 0)
    {
        if (argv[1] == NULL)
        {
            char *home = getenv("HOME");

            if (home == NULL)
            {
                fprintf(stderr, "cd: HOME not set\n");
                return 1;
            }

            if (chdir(home) != 0)
            {
                perror("cd");
                return 1;
            }
        }
        else if (argv[2] != NULL)
        {
            fprintf(stderr, "cd: too many arguments\n");
            return 1;
        }
        else
        {
            if (chdir(argv[1]) != 0)
            {
                perror("cd");
                return 1;
            }
        }

        return 0;
    }

    /* pwd */
    if (strcmp(argv[0], "pwd") == 0)
    {
        char cwd[4096];

        if (argv[1] != NULL)
        {
            fprintf(stderr, "pwd: too many arguments\n");
            return 1;
        }

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("pwd");
            return 1;
        }

        printf("%s\n", cwd);
        return 0;
    }

    /* echo */
    if (strcmp(argv[0], "echo") == 0)
    {
        int i = 1;

        while (argv[i] != NULL)
        {
            printf("%s", argv[i]);

            if (argv[i + 1] != NULL)
                printf(" ");

            i++;
        }

        printf("\n");
        return 0;
    }

    /* exit */
    if (strcmp(argv[0], "exit") == 0)
    {
        if (argv[1] != NULL)
        {
            fprintf(stderr, "exit: too many arguments\n");
            return 1;
        }

        return 1;
    }

    return 0;
}
