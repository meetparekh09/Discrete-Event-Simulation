/*
	Module Name: random.cpp
	Description: designed to load random numbers from rfile and generate random numbers as 		per burst provided.
*/
#include <fstream>


int *__numbers; //array that holds the numbers
int __size; //number of random numbers in rfile
int __curr; //current position in array


void load_random_numbers(char *filename) {
	/*
		Function Name: load_random_numbers
		Arguments:
			char *filename: path to rfile
		Returns: void
		Description: loads random numbers from rfile into __numbers array and initializes 		__size and __curr
	*/
	int num;

	//open file in std::ios_base::in mode to read integers from file.
	std::fstream file(filename, std::ios_base::in); 

	//first number read into __size
	file >> __size; 


	__numbers = new int[__size];
	__curr = 0;
	while(file >> num) {
		__numbers[__curr++] = num;
	}
	__curr = 0;
	return;
}


int myrandom(int burst) {
	/*
		Function Name: myrandom
		Arguments:
			int burst: burst size to be taken as modulus
		Returns: int - random burst
		Description: takes random number from array and generates the burst using burst.
	*/
	int num = __numbers[__curr];
	num = num % burst;
	__curr = (__curr + 1) % __size;
	return num;
}