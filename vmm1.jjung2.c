/**
 * Jay Hwasung Jung
 * CS 201 Operating Systems
 * Assignment 05 part I
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FRAMES 128
#define PAGE_SIZE 256
#define NUM_PAGES 256
#define BUFLEN 256


typedef struct {
    int pageTable[NUM_PAGES]; // the actual page table
    int accessTime[NUM_FRAMES]; // the most recent time that a frame was accessed
    unsigned char freeFrame[NUM_FRAMES]; // 0 if a frame is occupied; otherwise 1
} PageTableInfo;

// the function should take a four-byte integer in the range 0 to 256*256-1
// and compute the page number and page offset as described above;
// it should return 0 if the address argument is valid (in the range 0 to 256*256-1)
// and 1 otherwise
int decodeAddress(int address, int *pageNumber, int *pageOffset){
    if (address > PAGE_SIZE * (PAGE_SIZE - 1) || address < 0) {
        return 1;
    }
    else {
        *pageNumber = (address >> 8) & (PAGE_SIZE - 1);
        *pageOffset = address & (PAGE_SIZE - 1);
    }
    return 0;
}
// read bytes n to n+PAGE_SIZE-1, where n = PAGE_SIZE * pageNumber;
// put the data into the location pointed to by buffer
// return 0 if there was no error during the read; otherwise return 1
int readFromBackingStore(FILE *fp, char *buffer, int pageNumber) {
    int seek_result = fseek(fp, PAGE_SIZE * pageNumber, SEEK_SET);
    int read_result;
    if (seek_result != 0) {
        return 1;
    }
    else {
        read_result = fread(buffer, sizeof(char), PAGE_SIZE, fp);
        if (read_result != PAGE_SIZE) {
            return 1;
        }
        else {
            return 0;
        }
    }
}
// int main (void) {
//     int pageNum;
//     int pageOffset;
//     char *fname = "BACKING_STORE.dat";
//     char buffer[BUFLEN];

//     FILE *fp = fopen(fname, "r");
//     if (fp == NULL) {
//         printf("ERROR: cannot open file ’\%s’ for reading\n", fname);
//         exit(8); 
//     }

//     printf("%d\n", readFromBackingStore(fp, buffer, 255));


//     fclose(fp);
//     printf("%u\n", (unsigned char) buffer[227]);

//     return 0;
// }