/*
	Module Name: process.cpp
	Description: Loads and maintains data structure to hold processes and gives process 		metrics.
*/
#include <vector>
#include <fstream>
#include "scheduler.h"


std::vector<Process*> processes; //data structure that holds all processes.

/************************** Imported from random.cpp **************************/
extern int myrandom(int burst);

/************************** Imported from des.cpp **************************/
extern void put_event(Event *event, bool pushback);



void load_processes(char *filename) {
	/*
		Function Name: load_processes
		Arguments:
			char *filename: path to input file
		Returns: void
		Description: loads processes from file and initializes process data structure 			with processes
	*/

	//mode std::ios_base::in used for reading integers
	std::fstream file(filename, std::ios_base::in);

	Process *process;

	//pid initialized with zero and incremented with each process.
	int at, tc, cb, io, pid = 0;
	while(file >> at) {
		file >> tc;
		file >> cb;
		file >> io;
		process = new Process(at, tc, cb, io, pid++, myrandom(4)+1);
		processes.push_back(process);
		//generate event for create to ready
		Event *event = new Event(at, process, CREATE, READY);
		put_event(event, true);
	}
}

bool check_blocked() {
	/*
		Function Name: check_blocked
		Returns: bool - true if anyone process is in blocked state, false otherwise.
		Description: check if anyone process is in blocked state
	*/
	for(std::vector<Process*>::iterator i = processes.begin(); i != processes.end(); i++) {
		if((*i)->st == BLOCK) return true;
	}
	return false;
}


int get_total_time() {
	/*
		Function Name: get_total_time
		Returns: int - finish time of last executing process.
		Description: get the end time of simulation.
	*/
	int finish_time = 0;

	//basically checks for maximum finish time and returns the same.
	for(std::vector<Process*>::iterator i = processes.begin(); i != processes.end(); i++) {
		if(finish_time < (*i)->ft) {
			finish_time = (*i)->ft;
		}
	}
	return finish_time;
}

double get_cpu_utilization() {
	/*
		Function Name: get_cpu_utilization
		Returns: double - percent of time atleast one process is running
		Description: basically returns percent of time used in computation
	*/
	double tot_time = 0;

	//it sums all total_cpu_time of processes and divides it by simulation time to get the ratio of time used in computation and multiplies the same by 100 to get percentage
	for(std::vector<Process*>::iterator i = processes.begin(); i != processes.end(); i++) {
		tot_time += (*i)->tc;
	}
	double fin_time = (double) get_total_time();
	return (tot_time/fin_time)*100.00;
}


double get_avg_turnaround_time() {
	/*
		Function Name: get_avg_turnaround_time
		Returns: double - average turnaround time amongst process
		Description: computes average of turnaround time for all processes
	*/
	double tot_tt = 0;

	//compute average and return the same.
	for(std::vector<Process*>::iterator i = processes.begin(); i != processes.end(); i++) {
		tot_tt += (*i)->tt;
	}
	return (tot_tt/((double)processes.size()));
}

double get_avg_cpu_wait_time() {
	/*
		Function Name: get_avg_cpu_wait_time
		Returns: double - average cpu wait time amongst process
		Description: computes average cpu wait time amongst all processes
	*/
	double tot_cw = 0;

	//compute average and return the same.
	for(std::vector<Process*>::iterator i = processes.begin(); i != processes.end(); i++) {
		tot_cw += (*i)->cw;
	}
	return (tot_cw/((double)processes.size()));
}

double get_throughput() {
	/*
		Function Name: get_throughput
		Returns: double - number of processes completed per 100 time units
		Description: computes and returns throughput i.e. number of processes completed 		per 100 time unit
	*/
	double fin_time = get_total_time()/100.00; //get total number of 100 time units
	return (((double)processes.size())/fin_time); //return the ratio
}