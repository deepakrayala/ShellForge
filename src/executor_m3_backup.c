#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "executor.h"

int execute_external(char **argv)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        /*
         * Child process
         */
        execvp(argv[0], argv);

        /*
         * execvp() only returns when an error occurs
         */
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    /*
     * Parent process waits for child
     */
    int status;

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return -1;
    }

    return 0;
}
