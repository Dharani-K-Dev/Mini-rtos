#include "mini_rtos.h"
#include <stdio.h>

static void high_task(void *arg) {
    (void)arg;
    for (int i = 0; i < 3; i++) {
        printf("    [HIGH ] iteration %d\n", i);
        mini_delay(2);
    }
    printf("    [HIGH ] done\n");
}

static void med_task(void *arg) {
    (void)arg;
    for (int i = 0; i < 3; i++) {
        printf("    [MED  ] iteration %d\n", i);
        mini_delay(2);
    }
    printf("    [MED  ] done\n");
}

static void low_task(void *arg) {
    (void)arg;
    for (int i = 0; i < 3; i++) {
        printf("    [LOW  ] iteration %d\n", i);
        mini_yield();
    }
    printf("    [LOW  ] done\n");
}

int main(void) {
    mini_init();

    mini_task_create(high_task, NULL, 0, "high");
    mini_task_create(med_task,  NULL, 1, "med");
    mini_task_create(low_task,  NULL, 2, "low");

    mini_run();

    return 0;
}
