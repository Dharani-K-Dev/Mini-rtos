#include "mini_rtos.h"
#include <stdio.h>
#include <string.h>
#include <ucontext.h>

typedef struct {
    ucontext_t      context;
    uint8_t         stack[STACK_SIZE];
    mini_task_fn    fn;
    void           *arg;
    uint8_t         priority;
    task_state_t    state;
    uint32_t        delay_ticks;
    const char     *name;
} tcb_t;

static tcb_t    tasks[MAX_TASKS];
static int      task_count = 0;
static int      current_task = -1;
static ucontext_t scheduler_ctx;
static uint32_t tick = 0;

static void task_trampoline(void);

void mini_init(void) {
    task_count = 0;
    current_task = -1;
    tick = 0;
    memset(tasks, 0, sizeof(tasks));
}

int mini_task_create(mini_task_fn fn, void *arg, uint8_t priority, const char *name) {
    if (task_count >= MAX_TASKS) return -1;
    if (priority >= MAX_PRIORITY) return -1;

    tcb_t *t = &tasks[task_count];

    t->fn = fn;
    t->arg = arg;
    t->priority = priority;
    t->state = TASK_READY;
    t->delay_ticks = 0;
    t->name = name;

    getcontext(&t->context);
    t->context.uc_stack.ss_sp = t->stack;
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = &scheduler_ctx;
    makecontext(&t->context, task_trampoline, 0);

    task_count++;
    return 0;
}

static void task_trampoline(void) {
    tcb_t *t = &tasks[current_task];
    t->fn(t->arg);
    t->state = TASK_DEAD;
    mini_yield();
}
static int pick_next(void) {
    int best = -1;
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].state == TASK_READY) {
            if (best == -1 || tasks[i].priority < tasks[best].priority) {
                best = i;
            }
        }
    }
    return best;
}

static void update_delays(void) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].state == TASK_BLOCKED && tasks[i].delay_ticks > 0) {
            tasks[i].delay_ticks--;
            if (tasks[i].delay_ticks == 0) {
                tasks[i].state = TASK_READY;
            }
        }
    }
}

void mini_run(void) {
    printf("Mini-RTOS demo: %d tasks, cooperative scheduling\n", task_count);
    printf("---------------------------------------------------------------\n");

    while (1) {
        tick++;
        update_delays();

        int next = pick_next();
        if (next < 0) break;

        tasks[next].state = TASK_RUNNING;
        current_task = next;

        printf("[scheduler] tick=%-3u dispatch '%-5s' (pri=%d)\n",
               tick, tasks[next].name, tasks[next].priority);

        swapcontext(&scheduler_ctx, &tasks[next].context);

        if (tasks[next].state == TASK_RUNNING) {
            tasks[next].state = TASK_READY;
        }
    }

    printf("---------------------------------------------------------------\n");
    printf("All tasks finished. Total ticks: %u\n", tick);
}

void mini_yield(void) {
    swapcontext(&tasks[current_task].context, &scheduler_ctx);
}

void mini_delay(uint32_t ticks) {
    tasks[current_task].state = TASK_BLOCKED;
    tasks[current_task].delay_ticks = ticks;
    swapcontext(&tasks[current_task].context, &scheduler_ctx);
}
