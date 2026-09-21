#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "executor.h"
#include "builtin.h"

int execute_command(char *argv[])
{
    if (argv == NULL || argv[0] == NULL)
        return -1;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        char *exec_argv[MAX_TOKENS];
        int i = 0;

        while (argv[i] != NULL && i < MAX_TOKENS - 1)
        {
            exec_argv[i] = (char *)argv[i];
            i++;
        }

        exec_argv[i] = NULL;

        execvp(exec_argv[0], exec_argv);

        perror("execvp");
        _exit(127);
    }

    int status;

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return -1;
}

int execute_pipeline(pipeline_t *pipeline)
{
    if (pipeline == NULL || pipeline->command_count <= 0)
        return -2;

    /*
     * Single command.
     */
    if (pipeline->command_count == 1)
{
    char *argv[MAX_TOKENS];
    int i = 0;

    while (pipeline->commands[0][i] != NULL &&
           i < MAX_TOKENS - 1)
    {
        argv[i] = (char *)pipeline->commands[0][i];
        i++;
    }

    argv[i] = NULL;

    return execute_command(argv);
}
    int previous_read = -1;
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < pipeline->command_count; i++)
   {
        int pipefd[2];

        /*
         * Create a pipe for every command except the last.
         */
        if (i < pipeline->command_count - 1)
        {
            if (pipe(pipefd) < 0)
            {
                perror("pipe");
                return -1;
            }
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            return -1;
        }

        if (pid == 0)
        {
            /*
             * Connect previous pipe to stdin.
             */
            if (previous_read != -1)
            {
                if (dup2(previous_read, STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    _exit(127);
                }
            }

            /*
             * Connect current pipe to stdout.
             */
            if (i < pipeline->command_count - 1)
            {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0)
                {
                    perror("dup2");
                    _exit(127);
                }
            }

            /*
             * Close unused descriptors.
             */
            if (previous_read != -1)
                close(previous_read);

            if (i < pipeline->command_count - 1)
            {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            /*
             * Convert const command pointers into
             * the form required by execvp().
             */
            char *exec_argv[MAX_TOKENS];
            int j = 0;

            while (pipeline->commands[i][j] != NULL &&
                   j < MAX_TOKENS - 1)
            {
                exec_argv[j] =
                    (char *)pipeline->commands[i][j];

                j++;
            }

            exec_argv[j] = NULL;

            execvp(exec_argv[0], exec_argv);

            perror("execvp");
            _exit(127);
        }

        /*
         * Parent stores child PID.
         */
        pids[i] = pid;

        /*
         * Parent no longer needs previous pipe.
         */
        if (previous_read != -1)
            close(previous_read);

        /*
         * Keep read end for the next command.
         */
        if (i < pipeline->command_count - 1)
        {
            close(pipefd[1]);
            previous_read = pipefd[0];
        }
        else
        {
            previous_read = -1;
        }
    }

    /*
     * Wait for every child.
     * Return the exit status of the last command.
     */
    int last_status = 0;

    for (int i = 0; i < pipeline->command_count; i++)
    {
        int status;

        if (waitpid(pids[i], &status, 0) < 0)
        {
            perror("waitpid");
            return -1;
        }

        if (i == pipeline->command_count - 1)
        {
            if (WIFEXITED(status))
                last_status = WEXITSTATUS(status);
            else
                last_status = -1;
        }
    }

    return last_status;
}
