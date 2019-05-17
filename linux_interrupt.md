```
task_struct ->thread_info
int  preempt_count;    /* 0 => preemptable, <0 => bug */ 

preemption count：preempt_disable/enable

softirq count: local_bh_enable/disable
这时候，kernel提供了local_bh_enable和local_bh_disable这样的接口函数。
这部分的概念是和preempt disable/enable类似的，占用了bit9～15，最大可以支持127次嵌套 。 
local_bh_disable means enter softirq context(add 1 to bit9-15)

hardirq count: irq_enter/irq_exit， 
local_irq_enable/disable： asm volatile("sti/cli": : :"memory");


#define in_irq()        (hardirq_count())
#define in_softirq()        (softirq_count())
#define in_interrupt()        (irq_count())
#define in_serving_softirq()    (softirq_count() & SOFTIRQ_OFFSET)


// 不过，在其他context的情况下，例如进程上下文中，有有可能因为同步的要求而调用local_bh_disable，
// 这时候，通过local_bh_disable/enable保护起来的代码也是执行在softirq context中。当然，这时候其实并没有正在执行softirq handler。如果你确实想知道当前是否正在执行softirq handler，in_serving_softirq可以完成这个使命，这是通过操作preempt_count的bit 8来完成的。
// local_bh_enable/disable是给进程上下文使用的，用于防止softirq handler抢占local_bh_enable/disable之间的临界区的。 
```

```c
update_process_time()

            0 [004] 917530.321974: probe:update_process_times: (ffffffff81099130)
            7fff8109b131 update_process_times ([kernel.kallsyms])
            7fff810f4fc1 tick_sched_timer ([kernel.kallsyms])
            7fff810b6862 __hrtimer_run_queues ([kernel.kallsyms])
            7fff810b6e00 hrtimer_interrupt ([kernel.kallsyms])
            7fff810530d7 local_apic_timer_interrupt ([kernel.kallsyms])
            7fff8169accf smp_apic_timer_interrupt ([kernel.kallsyms]) //can ftrace
            7fff8169921d apic_timer_interrupt ([kernel.kallsyms])  // alloc_intr_gate(LOCAL_TIMER_VECTOR, apic_timer_interrupt);
            7fff81516199 cpuidle_idle_call ([kernel.kallsyms]) // driver
            7fff8103716e arch_cpu_idle ([kernel.kallsyms])
            7fff810e9c95 cpu_startup_entry ([kernel.kallsyms])
            7fff8105112a start_secondary ([kernel.kallsyms])


void account_process_tick(struct task_struct *p, int user_tick)
{
	cputime_t one_jiffy_scaled = cputime_to_scaled(cputime_one_jiffy);
	struct rq *rq = this_rq();

	if (vtime_accounting_enabled())
		return;

	if (sched_clock_irqtime) {
		irqtime_account_process_tick(p, user_tick, rq);
		return;
	}

	if (steal_account_process_tick())
		return;

	if (user_tick)
		account_user_time(p, cputime_one_jiffy, one_jiffy_scaled);
	else if ((p != rq->idle) || (irq_count() != HARDIRQ_OFFSET))
		account_system_time(p, HARDIRQ_OFFSET, cputime_one_jiffy,
				    one_jiffy_scaled);
	else
		account_idle_time(cputime_one_jiffy);
}

void account_idle_time(cputime_t cputime)
{
	u64 *cpustat = kcpustat_this_cpu->cpustat;
	struct rq *rq = this_rq();

	if (atomic_read(&rq->nr_iowait) > 0)
		cpustat[CPUTIME_IOWAIT] += (__force u64) cputime;
	else
		cpustat[CPUTIME_IDLE] += (__force u64) cputime;
}


void account_user_time(struct task_struct *p, cputime_t cputime,
		       cputime_t cputime_scaled)
{
	int index;

	/* Add user time to process. */
	p->utime += cputime;
	p->utimescaled += cputime_scaled;
	account_group_user_time(p, cputime);

	index = (TASK_NICE(p) > 0) ? CPUTIME_NICE : CPUTIME_USER;

	/* Add user time to cpustat. */
	task_group_account_field(p, index, (__force u64) cputime);

	/* Account for user time used */
	acct_account_cputime(p);
}

void account_system_time(struct task_struct *p, int hardirq_offset,
			 cputime_t cputime, cputime_t cputime_scaled)
{
	int index;

	if ((p->flags & PF_VCPU) && (irq_count() - hardirq_offset == 0)) {
		account_guest_time(p, cputime, cputime_scaled);
		return;
	}

	if (hardirq_count() - hardirq_offset)
		index = CPUTIME_IRQ;
	else if (in_serving_softirq())
		index = CPUTIME_SOFTIRQ;
	else
		index = CPUTIME_SYSTEM;

	__account_system_time(p, cputime, cputime_scaled, index);
}


static inline
void __account_system_time(struct task_struct *p, cputime_t cputime,
			cputime_t cputime_scaled, int index)
{
	/* Add system time to process. */
	p->stime += cputime;
	p->stimescaled += cputime_scaled;
	account_group_system_time(p, cputime);

	/* Add system time to cpustat. */
	task_group_account_field(p, index, (__force u64) cputime);

	/* Account for system time used */
	acct_account_cputime(p);
}

void __irq_entry smp_apic_timer_interrupt(struct pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs(regs);

	/*
	 * NOTE! We'd better ACK the irq immediately,
	 * because timer handling can be slow.
	 */
	ack_APIC_irq();
	/*
	 * update_process_times() expects us to have done irq_enter().
	 * Besides, if we don't timer interrupts ignore the global
	 * interrupt lock, which is the WrongThing (tm) to do.
	 */
	irq_enter();
	exit_idle();
	local_apic_timer_interrupt();
	irq_exit();

	set_irq_regs(old_regs);
}

```

``` /usr/local/bin/trace-cmd record -p function_graph -g hrtimer_interrupt  -O funcgraph-proc -F ping ** 
           <...>-69672 [002] 917795.467858: funcgraph_entry:                   |  hrtimer_interrupt() {
           <...>-69672 [002] 917795.467861: funcgraph_entry:        0.157 us   |    _raw_spin_lock();
           <...>-69672 [002] 917795.467862: funcgraph_entry:        0.260 us   |    ktime_get_update_offsets_now();
           <...>-69672 [002] 917795.467864: funcgraph_entry:                   |    __hrtimer_run_queues() {
           <...>-69672 [002] 917795.467864: funcgraph_entry:        0.267 us   |      __remove_hrtimer();
           <...>-69672 [002] 917795.467865: funcgraph_entry:        0.107 us   |      _raw_spin_unlock();
           <...>-69672 [002] 917795.467866: funcgraph_entry:                   |      tick_sched_timer() {
           <...>-69672 [002] 917795.467867: funcgraph_entry:        0.167 us   |        ktime_get();
           <...>-69672 [002] 917795.467868: funcgraph_entry:        0.120 us   |        tick_sched_do_timer();
           <...>-69672 [002] 917795.467869: funcgraph_entry:                   |        tick_sched_handle.isra.13() {
           <...>-69672 [002] 917795.467870: funcgraph_entry:                   |          update_process_times() {
           <...>-69672 [002] 917795.467870: funcgraph_entry:                   |            account_process_tick() {
           <...>-69672 [002] 917795.467871: funcgraph_entry:                   |              account_user_time() {
           <...>-69672 [002] 917795.467872: funcgraph_entry:        0.540 us   |                cpuacct_account_field();
           <...>-69672 [002] 917795.467873: funcgraph_entry:                   |                acct_account_cputime() {
           <...>-69672 [002] 917795.467874: funcgraph_entry:        0.196 us   |                  __acct_update_integrals();
           <...>-69672 [002] 917795.467875: funcgraph_exit:         1.216 us   |                }
           <...>-69672 [002] 917795.467875: funcgraph_exit:         3.833 us   |              }
           <...>-69672 [002] 917795.467876: funcgraph_exit:         4.992 us   |            }
           <...>-69672 [002] 917795.467876: funcgraph_entry:        0.126 us   |            hrtimer_run_queues();
           <...>-69672 [002] 917795.467877: funcgraph_entry:        0.126 us   |            raise_softirq();
           <...>-69672 [002] 917795.467878: funcgraph_entry:                   |            rcu_check_callbacks() {
           <...>-69672 [002] 917795.467879: funcgraph_entry:        0.190 us   |              cpu_needs_another_gp();
           <...>-69672 [002] 917795.467880: funcgraph_entry:                   |              invoke_rcu_core() {
           <...>-69672 [002] 917795.467881: funcgraph_entry:        0.144 us   |                raise_softirq();
           <...>-69672 [002] 917795.467882: funcgraph_exit:         1.280 us   |              }
           <...>-69672 [002] 917795.467882: funcgraph_exit:         3.636 us   |            }
           <...>-69672 [002] 917795.467883: funcgraph_entry:                   |            scheduler_tick() {
           <...>-69672 [002] 917795.467884: funcgraph_entry:        0.124 us   |              _raw_spin_lock();
           <...>-69672 [002] 917795.467885: funcgraph_entry:        0.184 us   |              update_rq_clock.part.78();
           <...>-69672 [002] 917795.467886: funcgraph_entry:        0.203 us   |              __update_cpu_load();
           <...>-69672 [002] 917795.467887: funcgraph_entry:                   |              task_tick_fair() {
           <...>-69672 [002] 917795.467888: funcgraph_entry:                   |                update_curr() {
           <...>-69672 [002] 917795.467888: funcgraph_entry:        0.070 us   |                  update_min_vruntime();
           <...>-69672 [002] 917795.467889: funcgraph_entry:        0.220 us   |                  cpuacct_charge();
           <...>-69672 [002] 917795.467890: funcgraph_exit:         1.610 us   |                }
           <...>-69672 [002] 917795.467890: funcgraph_entry:        0.133 us   |                update_cfs_rq_blocked_load();
           <...>-69672 [002] 917795.467891: funcgraph_entry:                   |                update_cfs_shares() {
           <...>-69672 [002] 917795.467891: funcgraph_entry:                   |                  update_curr() {
           <...>-69672 [002] 917795.467891: funcgraph_entry:        0.167 us   |                    __calc_delta();
           <...>-69672 [002] 917795.467892: funcgraph_entry:        0.094 us   |                    update_min_vruntime();
           <...>-69672 [002] 917795.467893: funcgraph_exit:         1.419 us   |                  }
           <...>-69672 [002] 917795.467893: funcgraph_entry:        0.087 us   |                  account_entity_dequeue();
           <...>-69672 [002] 917795.467893: funcgraph_entry:        0.094 us   |                  account_entity_enqueue();
           <...>-69672 [002] 917795.467894: funcgraph_exit:         3.262 us   |                }
           <...>-69672 [002] 917795.467894: funcgraph_entry:        0.097 us   |                update_curr();
           <...>-69672 [002] 917795.467895: funcgraph_entry:        0.093 us   |                update_cfs_rq_blocked_load();
           <...>-69672 [002] 917795.467896: funcgraph_entry:        0.134 us   |                update_cfs_shares();
           <...>-69672 [002] 917795.467896: funcgraph_entry:        0.086 us   |                task_tick_numa();
           <...>-69672 [002] 917795.467897: funcgraph_exit:         9.479 us   |              }
           <...>-69672 [002] 917795.467897: funcgraph_entry:        0.067 us   |              _raw_spin_unlock();
           <...>-69672 [002] 917795.467898: funcgraph_entry:                   |              trigger_load_balance() {
           <...>-69672 [002] 917795.467898: funcgraph_entry:        0.100 us   |                raise_softirq();
           <...>-69672 [002] 917795.467899: funcgraph_entry:        0.094 us   |                idle_cpu();
           <...>-69672 [002] 917795.467900: funcgraph_exit:         1.837 us   |              }
           <...>-69672 [002] 917795.467900: funcgraph_exit:       + 17.082 us  |            }
           <...>-69672 [002] 917795.467901: funcgraph_entry:                   |            run_posix_cpu_timers() {
           <...>-69672 [002] 917795.467901: funcgraph_entry:                   |              task_cputime() {
           <...>-69672 [002] 917795.467901: funcgraph_entry:        0.090 us   |                fetch_task_cputime();
           <...>-69672 [002] 917795.467902: funcgraph_exit:         0.657 us   |              }
           <...>-69672 [002] 917795.467903: funcgraph_exit:         1.397 us   |            }
           <...>-69672 [002] 917795.467903: funcgraph_exit:       + 32.695 us  |          }
           <...>-69672 [002] 917795.467903: funcgraph_entry:        0.120 us   |          profile_tick();
           <...>-69672 [002] 917795.467904: funcgraph_exit:       + 34.225 us  |        }
           <...>-69672 [002] 917795.467904: funcgraph_entry:        0.080 us   |        hrtimer_forward();
           <...>-69672 [002] 917795.467905: funcgraph_exit:       + 37.915 us  |      }
           <...>-69672 [002] 917795.467905: funcgraph_entry:        0.066 us   |      _raw_spin_lock();
           <...>-69672 [002] 917795.467906: funcgraph_entry:        0.263 us   |      enqueue_hrtimer();
           <...>-69672 [002] 917795.467907: funcgraph_exit:       + 42.512 us  |    }
           <...>-69672 [002] 917795.467907: funcgraph_entry:        0.130 us   |    __hrtimer_get_next_event();
           <...>-69672 [002] 917795.467908: funcgraph_entry:        0.070 us   |    _raw_spin_unlock();
           <...>-69672 [002] 917795.467909: funcgraph_entry:                   |    tick_program_event() {
           <...>-69672 [002] 917795.467909: funcgraph_entry:                   |      clockevents_program_event() {
           <...>-69672 [002] 917795.467910: funcgraph_entry:        0.100 us   |        ktime_get();
           <...>-69672 [002] 917795.467910: funcgraph_entry:        0.280 us   |        lapic_next_deadline();
           <...>-69672 [002] 917795.467911: funcgraph_exit:         1.507 us   |      }
           <...>-69672 [002] 917795.467911: funcgraph_exit:         2.173 us   |    }
           <...>-69672 [002] 917795.467912: funcgraph_exit:       + 51.338 us  |  }
```

