/*
	Module Name: scheduler.h
	Description: Contains definitions of all the classes to be used in the program
*/
#include <stdio.h>
#include <list>

#ifndef STATE_H
#define STATE_H

// to describe state of the process
enum State {CREATE, READY, RUNNING, BLOCK, PREEMPT};

#endif

#ifndef PROCESS_H
#define PROCESS_H

// for defining a process object
class Process {
public:
	int pid;
	int at;
	int tc;
	int cb;
	int io;
	int prio;
	int ft;
	int tt;
	int it;
	int cw;
	State st;
	
	// state timestamp to hold timestamp when it was transitioned into its current state
	int state_ts; 
	// time remaining to complete total cpu time
	int time_remaining;
	// current burst remaining time, used for preemptive algorithms
	int current_burst;
	// its current priority, used for PRIO scheduling where priority changes
	int current_prio;

	// Constructor
	Process(int at, int tc, int cb, int io, int pid, int prio) {
		this -> at = at;
		this -> tc = tc;
		this -> cb = cb;
		this -> io = io;
		this -> pid = pid;
		this -> prio = prio;
		ft = 0;
		tt = 0;
		it = 0;
		cw = 0;
		current_burst = 0;
		
		state_ts = at;
		st = CREATE;
		time_remaining = tc;
		current_prio = prio;
	}

	void print_process() {
		/*
			Function Name: print_process
			Returns: void
			Description: Prints the process and its calculated metrics in the format 			specified
		*/
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", pid, at, tc, cb, io, prio, ft, tt, it, cw);
	}
};


#endif


#ifndef EVENT_H
#define EVENT_H

// for defining an event
class Event {
public:
	int timestamp;
	Process *process;
	State oldstate;
	State newstate;

	// Constructor
	Event(int timestamp, Process *process, State oldstate, State newstate) {
		this->timestamp = timestamp;
		this->process = process;
		this->oldstate = oldstate;
		this->newstate = newstate;
	}

	void print() {
		/*
			Function Name: print
			Returns: void
			Description: prints an event, used only for debugging.
		*/
		printf("%d %d %d %d\n", timestamp, process->pid, oldstate, newstate);
	}

};

#endif

#ifndef SCHEDULER_H
#define SCHEDULER_H

// defines a generic scheduler to take advantage of polymorphism
class Scheduler {
public:
	// virtual function to be defined in child class
	virtual void add_process(Process *process) {
		return;
	}

	// virtual function to be defined in child class
	virtual Process* get_next_process() {
		return NULL;
	}
};

#endif

#ifndef FIFO_SCHEDULER_H
#define FIFO_SCHEDULER_H

// defines a FIFO Scheduler
class FIFOScheduler: public Scheduler {
private:
	std::list<Process*> run_queue; // data structure to maintain run_queue

public:
	void add_process(Process *process) {
		/*
			Function Name: add_process
			Arguments:
				Process *process: process to be added to the queue
			Returns: void
			Description: Inserts the process at the end of queue
		*/
		run_queue.push_back(process);
	}

	Process* get_next_process() {
		/*
			Function Name: get_next_process
			Returns: Process*: process to be executed next
			Description: returns the process at the front of the queue
		*/
		if(run_queue.size() == 0) {
			return NULL;
		}
		Process *process = run_queue.front();
		run_queue.pop_front();
		return process;
	}

};

#endif

#ifndef SJF_SCHEDULER_H
#define SJF_SCHEDULER_H

class SJFScheduler: public Scheduler {
private:
	std::list<Process*> run_queue; //data strucuture for run queue

public:

	void add_process(Process *process) {
		/*
			Function Name: add_process
			Arguments:
				Process *process: process to be added to the queue
			Returns: void
			Description: Inserts the process in sorted order of time_remaining
		*/
		if(run_queue.size() == 0) {
			run_queue.push_back(process);
			return;
		}
		std::list<Process*>::iterator i = run_queue.begin();
		while(i != run_queue.end() && process->time_remaining >= (*i)->time_remaining) {
			i++;
		}
		run_queue.insert(i, process);
		return;
	}

	Process* get_next_process() {
		/*
			Function Name: get_next_process
			Returns: Process*: process to be executed next
			Description: returns the process at the front of the queue
		*/
		if(run_queue.size() == 0) {
			return NULL;
		}
		Process *process = run_queue.front();
		run_queue.pop_front();
		return process;
	}
};

#endif

#ifndef LCFS_SCHEDULER_H
#define LCFS_SCHEDULER_H

class LCFSScheduler : public Scheduler {
private:
	std::list<Process*> run_queue;

public:
	void add_process(Process *process) {
		/*
			Function Name: add_process
			Arguments:
				Process *process: process to be added to the queue
			Returns: void
			Description: Inserts the process at the front of the queue
		*/
		run_queue.push_front(process);
	}

	Process* get_next_process() {
		/*
			Function Name: get_next_process
			Returns: Process*: process to be executed next
			Description: returns the process at the front of the queue
		*/
		if(run_queue.size() == 0) {
			return NULL;
		}
		Process *process = run_queue.front();
		run_queue.pop_front();
		return process;
	}
};

#endif

#ifndef PRIO_SCHEDULER_H
#define PRIO_SCHEDULER_H

class PRIOScheduler : public Scheduler {
private:
	// maintains two queue where one is active and other is inactive
	// queue is switched when no active processes remain in one
	std::list<Process*> queue1;
	std::list<Process*> queue2;

	// flag to indicate which queue is active
	bool is_queue1_active;

	void insert_queue1(Process *process) {
		/*
			Function Name: insert_queue1
			Arguments:
				Process *process: process to be added to the queue
			Returns: void
			Description: Inserts the process in queue 1 in the sorted order of current_prio
		*/
		std::list<Process*>::iterator i = queue1.begin();
		while(i != queue1.end() && (*i)->current_prio >= process->current_prio) {
			i++;
		}
		queue1.insert(i, process);
	}

	void insert_queue2(Process *process) {
		/*
			Function Name: insert_queue2
			Arguments:
				Process *process: process to be added to the queue
			Returns: void
			Description: Inserts the process in queue 2 in the sorted order of current_prio
		*/
		std::list<Process*>::iterator i = queue2.begin();
		while(i != queue2.end() && (*i)->current_prio >= process->current_prio) {
			i++;
		}
		queue2.insert(i, process);
	}

public:
	// Constructor
	PRIOScheduler() {
		is_queue1_active = true; // make queue 1 active.
	}

	void add_process(Process *process) {
		/*
			Function Name: add_process
			Arguments:
				Process *process: process to be added to the queue
			Returns: void
			Description: Insert the process in appropriate queue
		*/

		// if priority is zero then add to inactive queue else add to active queue
		if(process->current_prio == 0) {
			process->current_prio = process->prio;
			if(is_queue1_active) {
				insert_queue2(process);
			} else {
				insert_queue1(process);
			}
		} else {
			if(is_queue1_active) {
				insert_queue1(process);
			} else {
				insert_queue2(process);
			}
		}
	}

	Process* get_next_process() {
		/*
			Function Name: get_next_process
			Returns: Process* - process to be executed next
			Description: returns the first process from the active queue and flips the active flag if active queue is empty
		*/
		if(is_queue1_active && queue1.size() == 0) {
			is_queue1_active = false;
		}
		if(!is_queue1_active && queue2.size() == 0) {
			is_queue1_active = true;
		}
		if(is_queue1_active) {
			if(queue1.size() == 0) {
				return NULL;
			}
			Process *process = queue1.front();
			queue1.pop_front();
			return process;
		} else {
			if(queue2.size() == 0) {
				return NULL;
			}
			Process *process = queue2.front();
			queue2.pop_front();
			return process;
		}
	}
};

#endif