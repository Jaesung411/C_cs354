////////////////////////////////////////////////////////////////////////////////
// Main File:        mySigHandler.c
// This File:        mySigHandler.c
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
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int alarmDelayTime = 4; // alarm time
int counter = 0; // counter for SIGUSR

/*
 * Print the PID and current time every 4 seconds
 */
void handler_SIGALRM(){

    time_t currTime;

    printf("PID: %d ", getpid());

    if(time(&currTime) != -1){
        printf("CURRENT TIME: %s", ctime(&currTime));
    }else{
	printf("CURRENT TIME: INVALID");
    }

    alarm(alarmDelayTime);

}

/*
 * Send user message that received a SIGUSR signal
 */
void handler_SIGUSR1() {
    printf("SIGUSR1 handled and counted!\n");
    counter++;
}

/*
 * Print the total number of times handled SIGUSR1 signal and exit
 * When user types Ctrl + c
 */
void handler_SIGINT() {
    printf("\nSIGINT handled.\n");
    printf("SIGUSR1 was handled %i times. Exiting now.\n", counter);
    exit(0);
}

/*
 * handle the SIGALAM, SIGUSR1, and SIGINT signal
 * before user types Ctrl+c
 * argc command-line arguments passed by the user
 * argv array of character pointers listing all the arguments
 */
int main(){

    //Register a SIGALAM handler
    struct sigaction act1;
    memset(&act1, 0, sizeof(act1));
    act1.sa_flags = 0;
    act1.sa_handler = &handler_SIGALRM;

    //check sigaction
    if(sigaction(SIGALRM, &act1, NULL) != 0){
        printf("Error: SIGALRM handler.\n");
        exit(1);
    }

    //register a SIGUSR1 handler
    struct sigaction act2;
    memset(&act2, 0, sizeof(act2));
    act2.sa_flags = 0;
    act2.sa_handler = &handler_SIGUSR1;

    //check sigaction
    if(sigaction(SIGUSR1, &act2, NULL) != 0){
        printf("Error: SIGUSR1 handler.\n");
        exit(1);
    }

    //register a SIGINT handler
    struct sigaction act3;
    memset(&act3, 0, sizeof(act3));
    act3.sa_flags = 0;
    act3.sa_handler = &handler_SIGINT;

    //check sigaction
    if(sigaction(SIGINT, &act3, NULL) != 0) {
	printf("Error: SIGINT handler.\n");
	exit(1);
    }

    printf("pid and time print every 4 seconds.\n");
    printf("type ctrl-c to end the program.\n");

    alarm(alarmDelayTime);

    while(1){
    }
    return 0;
}
