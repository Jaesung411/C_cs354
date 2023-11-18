///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2022, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////

// add your own File Header information here
////////////////////////////////////////////////////////////////////////////////
// Main File:        myMagicSquare.c
// This File:        myMagicSquare.c
// Instructor:       deppeler
//
// Author:           Jaesung Lim
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Fully acknowledge and credit all sources of help,
//                   including Peer Mentors, Instructors, and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   Avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
    int size;           // dimension of the square
    int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
int getSize() {

    int size = 0;

    printf("Enter magic square's size(odd integer >=3)\n");
    scanf("%d", &size);

    if(size%2 == 0){
        printf("Magic square size must be odd\n");
        exit(1);
    }else if( size < 3){
        printf("Magic square size must be >= 3.\n");
        exit(1);
    }
    
    return size;
}

/* TODO:
 * Makes a magic square of size n using the alternate
 * Siamese magic square algorithm from assignment and
 * returns a pointer to the completed MagicSquare struct.
 *
 * n the number of rows and columns
 */
MagicSquare *generateMagicSquare(int n) {

    MagicSquare *pptr = malloc(sizeof(MagicSquare));
    (*pptr).size = n;
    (*pptr).magic_square = malloc(sizeof(int*)*n);
    int **sq;
    sq = (*pptr).magic_square;

    //make a board with all element as zero 
    for(int i = 0; i < n; i++){
        *(sq + i) = malloc(sizeof(int)*n);
    }
    
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            *(*(sq + i) + j) = 0;
        }
    }

    //Start by placing 1 at the center column of the topmost row in the square
    int row = n/2;
    int col = n-1;
    *(*(sq + row) + col) = 1;

    //start making magicsqaure
    for(int i = 2; i <= n*n; i++){
         
        int old_row = row;
        int old_col = col;

        row = row + 1;
        col = col + 1;

        //set the method in the last collumn or row
        if(old_row == n-1 && old_col == n-1) {
            row = old_row;
            col = old_col - 1;
        }else if(old_row == n-1){
            row = 0;
            col = old_col + 1;
        }else if(old_col == n-1){
            row = old_row + 1;
            col = 0;
        }

        //check if the next position is full
        if(*(*(sq + row) + col) == 0){
            *(*(sq + row) + col) = i;
        }else{
            *(*(sq + (old_row)) + old_col-1) = i;
            row = old_row;
            col = old_col-1;
        }
    
    }

    return pptr;
}
/* TODO:
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * magic_square the magic square to write to a file
 * filename the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {

    FILE *fp;
    fp = fopen(filename,"w");

    if (fp == NULL){
        printf("Can't open file for writing.\n");
        fclose(fp);
    }

    int size = (*magic_square).size;
    fprintf(fp,"%d\n",size);
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            fprintf(fp, "%d", *(*((*magic_square).magic_square + i) + j));
            if(j != size-1){
                fprintf(fp, ",");
            }else{
                fprintf(fp, "\n");
            }
        }
    }
    fclose(fp);

}

/* TODO:
 * Generates a magic square of the user specified size and
 * output the quare to the output filename
 *
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
    // TODO: Check input arguments to get output filename
    if(argc != 2)
    {
        printf("Usage: ./myMagicSquare <input_filename>\n");
        return 0;
    }
    
    // TODO: Get magic square's size from user
    int square_size = getSize();

    // TODO: Generate the magic square
    MagicSquare *square = generateMagicSquare(square_size);

    // TODO: Output the magic square
    fileOutputMagicSquare(square,*(argv+1));

    for(int i = 0; i < square_size; i++){
        free(*((*square).magic_square+i));
        *((*square).magic_square+i) = NULL;
    }
    
    free((*square).magic_square);
    (*square).magic_square = NULL;
    free(square);
    square = NULL;

    return 0;
}


//     F22 deppeler myMagicSquare.c
