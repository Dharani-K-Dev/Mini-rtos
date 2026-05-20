#ifndef MINI_RTOS_H
#define MINI_RTOS_H

#include <stdint.h>

#define MAX_TASKS       8
#define STACK_SIZE      (64 * 1024)
#define MAX_PRIORITY    4

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_DEAD
} task_state_t;

typedef void (*mini_task_fn)(void *arg);

void mini_init(void);
int  mini_task_create(mini_task_fn fn, void *arg, uint8_t priority, const char *name);
void mini_run(void);
void mini_yield(void);
void mini_delay(uint32_t ticks);

#endif
