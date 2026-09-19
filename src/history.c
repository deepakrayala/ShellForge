#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"

#define MAX_HISTORY 100

typedef struct history_node {
    char *command;
    struct history_node *next;
} history_node_t;

static history_node_t *history_head = NULL;
static history_node_t *history_tail = NULL;
static int history_count = 0;

void history_add(const char *command)
{
    if (command == NULL || command[0] == '\0')
        return;

    history_node_t *node = malloc(sizeof(history_node_t));

    if (node == NULL)
    {
        fprintf(stderr, "History Error: memory allocation failed\n");
        return;
    }

    node->command = malloc(strlen(command) + 1);

    if (node->command == NULL)
    {
        fprintf(stderr, "History Error: memory allocation failed\n");
        free(node);
        return;
    }

    strcpy(node->command, command);
    node->next = NULL;

    if (history_tail == NULL)
    {
        history_head = node;
        history_tail = node;
    }
    else
    {
        history_tail->next = node;
        history_tail = node;
    }

    history_count++;

    /*
     * Keep only the most recent MAX_HISTORY commands.
     */
    if (history_count > MAX_HISTORY)
    {
        history_node_t *old = history_head;

        history_head = history_head->next;

        free(old->command);
        free(old);

        history_count--;
    }
}

void history_print(void)
{
    history_node_t *current = history_head;
    int number = 1;

    printf("\n----------- HISTORY -----------\n");

    while (current != NULL)
    {
        printf("%4d  %s\n", number, current->command);

        current = current->next;
        number++;
    }

    printf("-------------------------------\n");
}

void history_free(void)
{
    history_node_t *current = history_head;

    while (current != NULL)
    {
        history_node_t *next = current->next;

        free(current->command);
        free(current);

        current = next;
    }

    history_head = NULL;
    history_tail = NULL;
    history_count = 0;
}
