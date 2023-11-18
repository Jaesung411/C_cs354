////////////////////////////////////////////////////////////////////////////////
// Main File:        mySigHandler.c
// This File:        division.c
// Other Files:      division.c mySigHandler.c sendsig.c
// Semester:         CS 354 Lecture 00? Fall 2022
// Instructor:       deppeler
// 
// Author:           Jaesung Lim
// Email:            jlim83@wisc.edu
// CS Login:         jaesung
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int counter = 0;

/*
 * Handler for SIGFPE signal
 *
 * When the user tries to divide by 0
 * Print appropriate error message, the total number of operation, and exit
 */
void handler_SIGFPE() {
	printf("Error: a division by 0 operation was attempted.\n");
    	printf("Total number of operations completed successfully: %i\n", 
			counter);
    	printf("The program will be terminated.\n");
    	exit(0);
}

/*
 * Handler for SIGINT signal
 *
 * When the user types Ctrl+c
 * Print the total number of operation, and exit
 */
void handler_SIGINT() {
    	printf("\nTotal number of operation completed successfully: %i\n", 
			counter);
    	printf("The program will be terminated.\n");
    	exit(0);
}

/*
 * During infinite loop, do operation on users' inputs
 *
 * User enter two integers
 * argc command-line arguments passed by the user 
 * argv array of character pointers listing all the arguments
 * 
 */
int main(int argc, char *argv[]) {

	//first and second input from user
	char strFirst[100];
	char strSecond[100];

	//first and second integer, mod, and remainder
	int int1;
	int int2;
	int result;
	int remainder;

	//register a SIGFPE handler
    	struct sigaction act1;
    	memset(&act1, 0, sizeof(act1));
   	act1.sa_flags = 0;
	act1.sa_handler = handler_SIGFPE;

	//check sigaction
    	if(sigaction(SIGFPE, &act1, NULL) != 0) {
        	printf("Error: SIGFPE handler.\n");
        	exit(1);
    	}

	//register a SIGINT handler
    	struct sigaction act2;
    	memset(&act2, 0, sizeof(act2));
    	act2.sa_flags = 0;
	act2.sa_handler = handler_SIGINT;

    	//check sigation
	if(sigaction(SIGINT, &act2, NULL) != 0) {
		printf("Error: SIGINT handler.\n");
		exit(1);
    	}

   	while(1) {
	 	//get the first integer from user
        	printf("Enter first integer: ");
        	if(fgets(strFirst, 100, stdin) != NULL){
            		int1 = atoi(strFirst);
		}else {
	    		printf("fgets failed.");
	    		exit(1);
		}

		//get the second integer from user
		printf("Enter second integer: ");
        	if(fgets(strSecond, 100, stdin) != NULL){
            		int2 = atoi(strSecond);
		}else {
	    		printf("fgets failed.");
            		exit(1);
        	}

		//caculate and print the result
        	result = int1 / int2;
        	remainder = int1 % int2;
        	printf("%i / %i is %i with a remainder of %i\n",
			       	int1, int2, result, remainder);
        	counter++;
    	}
	return 0;
}
