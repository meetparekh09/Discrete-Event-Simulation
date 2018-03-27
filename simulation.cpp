#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "scheduler.h"

/************************** Imported from des.cpp **************************/
extern Event* get_event();
extern void put_event(Event *event, bool pushback);
extern int get_next_event_time();

/************************** Imported from random.cpp **************************/
extern int myrandom(int burst);

/************************** Imported from process.cpp **************************/
extern std::vector<Process*> processes;
extern int get_total_time();
extern double get_cpu_utilization();
extern double get_avg_turnaround_time();
extern double get_avg_cpu_wait_time();
extern double get_throughput();
extern bool check_blocked();



void change_state(Process *proc, State state, int timestamp);

// for printing state if verbose
const char *s[] = {"CREATE", "READY", "RUNNING", "BLOCK", "PREEMPT"};

void simulate(char sched, int num, bool verbose) {
	/*
		Function Name: simulate
		Arguments:
			char sched: Scheduler to be used
			int num: time quantum
			bool verbose: true if -v option is select false otherwise
		Returns: void
		Description: Simulates the scheduling.
	*/
	Event *event = NULL; // for holding current event
	Process *current_running_process = NULL; // holds current_running_process
	Process *proc = NULL; // holds process in current event
	int current_time; // timestamp of current event
	int prev_state_time; // holds time process spent in previous state
	Scheduler *scheduler = NULL; // refers to scheduler
	bool call_scheduler = false; // flag determining whether to call scheduler
	double tot_io_time = 0.0; // calculates time anyone process is in io
	bool is_io = false; // flag to indicate anyone process is in io
	double prev_io_st_time = 0.0; // time is_io flag was set
	int io_burst, cpu_burst; // holds burst for cpu and io
	bool preempt = false; // flag to determine whether to preempt or block

	// initialize scheduler as per sched argument
	if(sched == 'F' || sched == 'R') {
		scheduler = new FIFOScheduler();
	} else if(sched == 'S'){
		scheduler = new SJFScheduler();
	} else if(sched == 'L') {
		scheduler = new LCFSScheduler();
	} else if(sched == 'P') {
		scheduler = new PRIOScheduler();
	} else {
		return;
	}


	// run while event queue is empty
	while(event = get_event()) {
		// get details of event and process
		State transition = event->newstate;
		State prev_state = event->oldstate;
		current_time = event->timestamp;
		proc = event->process;
		prev_state_time = current_time - proc->state_ts;

		switch(transition) {
			case READY:
			{
				change_state(proc, READY, current_time); //chage state to ready
				
				// set is io to false if no more process is blocked for io
				// add the time to tot_io_time
				if(prev_state == BLOCK){
					is_io = check_blocked();
					if(!is_io) {
						tot_io_time += (double)current_time - prev_io_st_time;
					}
				}

				// add process to run queue
				scheduler->add_process(proc);
				call_scheduler = true;
				break;
			}
			case RUNNING:
			{
				// if previos state was ready then add prev_state_time to time waiting for cpu
				if(prev_state == READY) {
					proc->cw += prev_state_time;
				}

				// calculates current cpu burst as per scheduler
				if(sched == 'R' || sched == 'P') {
					if(proc->current_burst == 0) {
						cpu_burst = myrandom(proc->cb)+1;
						proc->current_burst = cpu_burst;
						preempt = true;
						if(num >= cpu_burst) {
							preempt = false;
						} else {
							cpu_burst = num;
						}
					} else {
						if(proc->current_burst <= num) {
							cpu_burst = proc->current_burst;
						} else {
							cpu_burst = num;
							preempt = true;
						}
					}

					// if scheduler is PRIO then decrease current priority by 1
					if(sched == 'P') {
						proc->current_prio -= 1; 
					}
					proc->current_burst -= cpu_burst;
				} else {
					cpu_burst = myrandom(proc->cb)+1;
				}
				
				if(proc->time_remaining < cpu_burst) {
					cpu_burst = proc->time_remaining;
					call_scheduler = true;
				}
				proc->time_remaining -= cpu_burst;
				change_state(proc, RUNNING, current_time); //change state to running
				if(proc->time_remaining == 0) {
					proc->ft = current_time+cpu_burst;
					proc->tt = proc->ft-proc->at;
				}

				// put an event for preemption or blocking
				if(preempt) {
					put_event(new Event(current_time+cpu_burst, proc, RUNNING, PREEMPT), false);
					preempt = false;
				} else {
					put_event(new Event(current_time+cpu_burst, proc, RUNNING, BLOCK), false);
				}
				break;
			}
			case BLOCK:
			{
				// change current_prio to original one if scheduler is PRIO
				if(sched == 'P') {
					proc->current_prio = proc->prio;
				}
				current_running_process = NULL; // no process is running now
				call_scheduler = true;

				// no need to put event for READY or calculate the burst if process is complete
				if(proc->time_remaining == 0) {
					break;
				}

				// set is_io if not set
				if(!is_io) {
					is_io = true;
					prev_io_st_time = current_time;
				}
				io_burst = myrandom(proc->io)+1;
				proc->it += io_burst;
				change_state(proc, BLOCK, current_time);
				put_event(new Event(current_time+io_burst, proc, BLOCK, READY), false);
				break;
			}
			case PREEMPT:
			{
				current_running_process = NULL; // no process is running now
				call_scheduler = true;
				// no need to preempt if process is complete
				if(proc->time_remaining == 0) {
					break;
				}
				scheduler->add_process(proc);
				change_state(proc, READY, current_time);
				break;
			}
		}


		/************* Verbose Print ***************/
		if(verbose) {
			if(transition == BLOCK) {
				printf("%d %d %d: %s -> %s ib=%d rem=%d\n", current_time, proc->pid, prev_state_time, s[prev_state], s[transition], io_burst, proc->time_remaining);
			} else if(transition == RUNNING) {
				printf("%d %d %d: %s -> %s cb=%d rem=%d, current_burst=%d, prio=%d\n", current_time, proc->pid, prev_state_time, s[prev_state], s[transition], cpu_burst, proc->time_remaining, proc->current_burst, proc->current_prio);
			} else {
				printf("%d %d %d: %s -> %s\n", current_time, proc->pid, prev_state_time, s[prev_state], s[transition]);
			}
		}
		/************* Verbose Print End ***************/

		delete event;
		event = NULL;

		// Call scheduler
		if(call_scheduler) {
			if(get_next_event_time() == current_time) {
				continue;
			}
			call_scheduler = false;
			if(current_running_process == NULL) {
				current_running_process = scheduler->get_next_process();
				if(current_running_process == NULL) {
					continue;
				}
			} else {
				continue;
			}
			put_event(new Event(current_time, current_running_process, current_running_process->st, RUNNING), false);
		}
	}

	if(is_io) {
		tot_io_time = current_time - prev_io_st_time;
		is_io = false;
	}


	/*********************** Print Output *********************/
	if(sched == 'F') {
		printf("FCFS\n");
	} else if(sched == 'S') {
		printf("SJF\n");
	} else if(sched == 'L') {
		printf("LCFS\n");
	} else if(sched == 'R') {
		printf("RR %d\n", num);
	} else if(sched == 'P') {
		printf("PRIO %d\n", num);
	}



	for(int i = 0; i < processes.size(); i++) {
		processes.at(i)->print_process();
	}
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
		get_total_time(),
		get_cpu_utilization(),
		(tot_io_time/(double)get_total_time())*100.00,
		get_avg_turnaround_time(),
		get_avg_cpu_wait_time(),
		get_throughput());


}


void change_state(Process *proc, State state, int timestamp) {
	/*
		Function Name: change_state
		Arguments:
			Process *proc: process of which state is to be changed
			State state: state to be assigned to proc
			int timestamp: time at which state is changed
		Returns: void
		Description: Changes the state of the process as specified
	*/
	proc->st = state;
	proc->state_ts = timestamp;
	return;
}