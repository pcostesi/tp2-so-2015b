#ifndef __SCHED
#define __SCHED 1

#include <stdint.h>
#include <moduleLoader.h>
#define SCHED_MAX_PROC (20)

typedef uint64_t pid_t;
typedef uint64_t size_t;

enum sched_sleeping {
	IO,
	WAIT,
	TIME
};

uint64_t sched_switch_to_kernel_stack(uint64_t stack);
uint64_t sched_spawn_module(struct module_entry * entry, void * symbol);
uint64_t sched_switch_to_user_stack(uint64_t stack);
uint64_t _sched_get_current_process_entry(void);
uint64_t sched_init(void * pagetable);
uint64_t sched_terminate_process(pid_t pid, unsigned short retval);
pid_t sched_getpid(void);
uint64_t sched_get_process(void);

extern void sched_drop_to_user(void);
extern void sched_step_syscall_rax(void * stack, uint64_t value);

#endif