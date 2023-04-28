/**
 * Jay Hwasung Jung
 * CS 201 Operating Systems
 * Assignment 05 part II
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

char physicalMemory[NUM_FRAMES * PAGE_SIZE];

// the function should take a four-byte integer in the range 0 to 256*256-1
// and compute the page number and page offset as described above;
// it should return 0 if the address argument is valid (in the range 0 to 256*256-1)
// and 1 otherwise
int decodeAddress(int address, int *pageNumber, int *pageOffset){
    if (address > PAGE_SIZE * PAGE_SIZE - 1 || address < 0) {
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
int getFrameNumber(PageTableInfo *pageTableInfo, int logicalPageNumber, int accessTime, int *pageFault) {
    int found = 1;
    int frameNum = 0;
    int tempAccessTime = NUM_FRAMES * PAGE_SIZE;
    int pageStorage = -1;
    int count = 0;
    for (int i = 0; i < NUM_FRAMES; i ++) {
        if (pageTableInfo->freeFrame[i] == '0') {
            count ++;
        }
    }
    // if the page is in memory
    if (pageTableInfo->pageTable[logicalPageNumber] != -1) {
        // in this case, frame number is in pageTable
        frameNum = pageTableInfo->pageTable[logicalPageNumber];
        
        // update accessTime
        pageTableInfo->accessTime[frameNum] = accessTime;

        // no page Fault
        *pageFault = 0;
        

    }
    // else - page Fault
    else {

        if (count == NUM_FRAMES) {

            // get the smallest accessTime
            for (int i = 0; i < NUM_FRAMES; i ++) {
                if (tempAccessTime > pageTableInfo->accessTime[i]) {
                    frameNum = i;
                    tempAccessTime = pageTableInfo->accessTime[i];
                }
            }            
            for (int i = 0; i < NUM_PAGES; i ++) {
                if (frameNum == pageTableInfo->pageTable[i]) {
                    pageStorage = i;
                }
            }
            // evict when frames are full
            printf("  => EVICT! oldest frame is %d (age = %d)\n", frameNum, tempAccessTime);
            printf("  => the page mapped to frame %d is %d: page %d is now unmapped (not in memory)\n", frameNum, pageStorage, pageStorage);
            
            // change accessTime and freeFrame
            pageTableInfo->freeFrame[frameNum] = '1';
            pageTableInfo->accessTime[frameNum] = -1;
            pageTableInfo->pageTable[pageStorage] = -1;
        }

        while (found) {
            // find free page
            if (pageTableInfo->freeFrame[frameNum] == '1') {
                pageTableInfo->freeFrame[frameNum] = '0';
                pageTableInfo->pageTable[logicalPageNumber] = frameNum;
                pageTableInfo->accessTime[frameNum] = accessTime;
                found = 0;
                *pageFault = 1;
            }
            else {
                frameNum += 1;
            }
        }
    }
    return frameNum;
}
int main (void) {
    int pageNum;
    int pageOffset;
    char *fname = "BACKING_STORE.dat";
    char buffer[BUFLEN];
    char addresses[BUFLEN];

    PageTableInfo pageTableInfo;
    
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("ERROR: cannot open file ’\%s’ for reading\n", fname);
        exit(8); 
    }

    

    // initialization
    for (int i = 0; i < NUM_FRAMES; i ++){
        pageTableInfo.accessTime[i] = -1;
        pageTableInfo.freeFrame[i] = '1';
    }
    for (int i = 0; i < NUM_PAGES; i ++){
        pageTableInfo.pageTable[i] = -1;
    }


    // while loop
    int currentTime = 0;
    int accessTime = 0;
    char *chp;
    int tempAddress = 0;
    int pageFault;
    int frameNum = 0;
    int actual = 0;
    FILE *fptxt = fopen("addresses.txt", "r");


    if (fptxt == NULL) {
        printf("ERROR: cannot open file ’addresses.txt’ for reading\n");
    }

    chp = fgets(addresses, sizeof(addresses), fptxt);
    addresses[strlen(addresses)-1] = '\0';
    currentTime += 1;

    while (chp != NULL) {
        actual = 0;
        tempAddress = atoi(addresses);

        // decode

        decodeAddress (tempAddress, &pageNum, &pageOffset);
        frameNum = getFrameNumber (&pageTableInfo, pageNum, accessTime, &pageFault);

        // page fault 
        // get data from 
        if (pageFault == 1) {
            readFromBackingStore (fp, buffer, pageNum);
            for (int i = 0; i < PAGE_SIZE; i ++) {
                physicalMemory[i + frameNum * PAGE_SIZE] = buffer[i];
            }
        }
        actual = frameNum * PAGE_SIZE + pageOffset;
        
        // print
        if (pageFault) printf("* ");
        else printf("  ");
        printf("Virtual address: %d [%d, %d] Physical address: %d [%d, %d] Value: %u\n", tempAddress, pageNum, pageOffset, actual, frameNum, pageOffset, (unsigned char)physicalMemory[actual]);
        accessTime += 1;

        // get next address
        chp = fgets(addresses, sizeof(addresses), fptxt);
        addresses[strlen(addresses)-1] = '\0';
        currentTime += 1;
    }
    fclose (fptxt);  
    fclose(fp);
    
    return 0;
}