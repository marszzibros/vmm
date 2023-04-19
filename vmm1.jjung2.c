#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FRAMES 128
#define PAGE_SIZE 256
#define NUM_PAGES 256

int accessTime[NUM_FRAMES];

// the function should take a four-byte integer in the range 0 to 256*256-1
// and compute the page number and page offset as described above;
// it should return 0 if the address argument is valid (in the range 0 to 256*256-1)
// and 1 otherwise
int decodeAddress(int address, int *pageNumber, int *pageOffset){
    if (address > 256*(256 - 1) || address < 0) {
        return 1;
    }
    else {
        *pageNumber = (address >> 8) & (256 - 1);
        *pageOffset = address & (256 - 1);
    }
    return 0;
}
// // read bytes n to n+PAGE_SIZE-1, where n = PAGE_SIZE * pageNumber;
// // put the data into the location pointed to by buffer
// // return 0 if there was no error during the read; otherwise return 1
// int readFromBackingStore(FILE *fp, char *buffer, int pageNumber) { 

// }
int main (void) {
    int pageNum;
    int pageOffset;
    char *fname = "BACKING_STORE.dat";
    char *buffer;
    FILE *fp = fopen(fname, "rb");
    if (fp == NULL) {
        printf("ERROR: cannot open file ’\%s’ for reading\n", fname);
        exit(8); 
    }
    int num = 0;

    fread(&buffer, 16, 256, fp);

    printf("%s\n", buffer);


    fclose(fp);

    return 0;
}