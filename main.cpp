/*
	Module Name: main.cpp
	Description: main module which invokes other modules and coordinates the same.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/************************** Imported from random.cpp **************************/
extern void load_random_numbers(char *filename);

/************************** Imported from process.cpp **************************/
extern void load_processes(char *filename);

/************************** Imported from simulation.cpp **************************/
extern void simulate(char sched, int num, bool verbose);

int main(int argc, char *argv[]) {
	/*
		Function Name: main
		Arguments:
			int argc: number of command line arguments
			char *argv[]: strings having command line arguments
		Returns: int - status of execution
		Description: main module where execution begins, it parses command line arguments 		and coordinates activities between different modules to run simulation.
	*/

	int opt; //option from command line argument
	int num = -1; //required only if -s option is 'R' or 'P'
	bool verbose = false; //check presend of -v option
	char sched = 'n'; //just a initialization

	//read for options
	while((opt = getopt(argc, argv, "vs:")) != -1) {
		switch(opt) {
		case 'v':
			verbose = true;
			break;
		case 's':
			if(optarg != NULL) sched = optarg[0];
			if(sched == 'R' || sched == 'P') {
				num = atoi(optarg+1);
			}
			break;
		default:
			printf("Invalid Option\n");
		}
	}
	
	// optind gives location of other command line arguments other than options. 
	// optind+1 would have file path for rfile
	load_random_numbers(argv[optind+1]);

	// loads process and passes optind pointing to filepath for input file.
	load_processes(argv[optind]);

	//start simulation
	simulate(sched, num, verbose);
	
	return 0;
}