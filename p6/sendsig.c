////////////////////////////////////////////////////////////////////////////////
// Main File:        mySigHandler.c
// This File:        sendsig
// Other Files:      division.c mySigHandler.c sendsig.c
// Semester:         CS 354 Lecture 001 Fall 2022
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
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/*
 * if user input -u, call SIGUSR1, and if user input -i, call SIGINT.
 * if users type incorrect argument, send error message
 *
 * user type appropriate command 
 * argc command-line arguments passed by the user
 * argv array of character pointers listing all the arguments
 */
int main(int argc, char *argv[]) {

     //if argument is not correct, send error message
     if(argc != 3){
	 printf("Usage: sendsig <signal type> <pid>\n");
	 exit(1);
     }

     //send a SIGUSR1 signal if the argument is -u 
     if(strcmp(argv[1], "-u") == 0){
	if(kill(atoi(argv[2]), SIGUSR1) == -1){
		printf("Error: kill\n");
		exit(1);
	}
     //send a SIGINT signal if the argument is -i
     }else if(strcmp(argv[1], "-i") == 0){
	     if(kill(atoi(argv[2]), SIGINT) == -1){
		     printf("Error: kill\n");
		     exit(1);
	     }
     }
     return 0;
}
