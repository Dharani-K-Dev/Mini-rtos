# Mini Cooperative RTOS Scheduler

A minimal cooperative task scheduler in pure C that demonstrates the core internals of every RTOS — tasks, task control blocks (TCB), ready queue, priority-based selection, and yield-driven context switching.

## What This Demonstrates

- **Task Control Blocks (TCB):** Static array of 8 TCBs with function pointer, priority, state, private stack, and saved context
- **4 Task States:** READY, RUNNING, BLOCKED, DEAD — with proper state transitions
- **Priority-Based Scheduling:** Lower number = higher priority. Scheduler always picks the highest-priority READY task
- **Cooperative Context Switching:** Tasks voluntarily yield or delay. Uses POSIX `ucontext_t` / `swapcontext()` (portable equivalent of FreeRTOS PendSV assembly)
- **Blocking Delay:** `mini_delay(ticks)` moves task to BLOCKED state, decremented each tick, auto-wakes to READY

## Public API

```c
void mini_init(void);
int  mini_task_create(mini_task_fn fn, void *arg, uint8_t priority, const char *name);
void mini_run(void);
void mini_yield(void);
void mini_delay(uint32_t ticks);
```

## Build & Run

```bash
make run
```

## Sample Output

```
Mini-RTOS demo: 3 tasks, cooperative scheduling
---------------------------------------------------------------
[scheduler] tick=1   dispatch 'high ' (pri=0)
    [HIGH ] iteration 0
[scheduler] tick=2   dispatch 'med  ' (pri=1)
    [MED  ] iteration 0
[scheduler] tick=3   dispatch 'high ' (pri=0)
    [HIGH ] iteration 1
[scheduler] tick=4   dispatch 'med  ' (pri=1)
    [MED  ] iteration 1
[scheduler] tick=5   dispatch 'high ' (pri=0)
    [HIGH ] iteration 2
[scheduler] tick=6   dispatch 'med  ' (pri=1)
    [MED  ] iteration 2
[scheduler] tick=7   dispatch 'high ' (pri=0)
    [HIGH ] done
[scheduler] tick=8   dispatch 'med  ' (pri=1)
    [MED  ] done
[scheduler] tick=9   dispatch 'low  ' (pri=2)
    [LOW  ] iteration 0
[scheduler] tick=10  dispatch 'low  ' (pri=2)
    [LOW  ] iteration 1
[scheduler] tick=11  dispatch 'low  ' (pri=2)
    [LOW  ] iteration 2
[scheduler] tick=12  dispatch 'low  ' (pri=2)
    [LOW  ] done
---------------------------------------------------------------
All tasks finished. Total ticks: 13
```

**Key observation:** `high` always runs when READY. `med` only runs while `high` is BLOCKED. `low` only runs after both higher-priority tasks finish. This is exactly how preemptive FreeRTOS behaves — demonstrated here without preemption.

## How It Maps to FreeRTOS

| This Project | FreeRTOS Equivalent |
|---|---|
| `mini_task_create()` | `xTaskCreate()` |
| `mini_run()` | `vTaskStartScheduler()` |
| `mini_yield()` | `taskYIELD()` |
| `mini_delay(ticks)` | `vTaskDelay()` |
| `ucontext_t` / `swapcontext()` | PendSV assembly (PUSH/POP R4-R11) |
| `tcb_t` struct | FreeRTOS TCB (`tskTCB`) |
| `pick_next()` | `prvSelectHighestPriorityTask()` |

## Project Structure

```
freertos-sim/
├── include/
│   └── mini_rtos.h      # Public API and type definitions
├── src/
│   ├── mini_rtos.c       # Scheduler implementation
│   └── main.c            # Demo: 3 tasks at 3 priorities
├── Makefile
└── README.md
```

## Roadmap

- [ ] Counting semaphores
- [ ] Mutex with priority inheritance
- [ ] Message queues
- [ ] Stack overflow detection (canary pattern)
- [ ] Round-robin for equal-priority tasks

## Author

**Dharani K** — Embedded & Firmware Engineer
- GitHub: [@Dharani-K-Dev](https://github.com/Dharani-K-Dev)
- LinkedIn: [dharani-kumaresan-embedded](https://www.linkedin.com/in/dharani-kumaresan-embedded/)
