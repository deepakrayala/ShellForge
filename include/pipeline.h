#ifndef PIPELINE_H
#define PIPELINE_H

#include "token.h"

#define MAX_COMMANDS 64

typedef struct
{
    const  char *commands[MAX_COMMANDS][MAX_TOKENS];
    int argc[MAX_COMMANDS];
    int command_count;
} pipeline_t;

int pipeline_build(const token_list_t *tokens, pipeline_t *pipeline);

#endif
