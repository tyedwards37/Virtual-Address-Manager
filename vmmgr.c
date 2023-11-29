#include <stdio.h>
#include <stdlib.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 28
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 65536

typedef struct 
{
    int pageNumber;
    int frameNumber;
} tlbEntry;

typedef struct 
{
    int frameNumber;
    int valid;
} pageTableEntry;

void extractPageAndOffset(int logicalAddress, int* pageNumber, int* offset);
int tlbHit(int pageNumber, tlbEntry* tlb);
int getFrameNumberFromTLB(int pageNumber, tlbEntry* tlb);
int getFrameNumberFromPageTable(int pageNumber, pageTableEntry* pageTable);
void handlePageFault(int pageNumber, pageTableEntry* pageTable, int* physicalMemory);
void updateTLB(int pageNumber, int frameNumber, tlbEntry *tlb);
int pageNumberHit(int pageNumber, pageTableEntry* pageTable);
void updatePageTable(int pageNumber, int frameNumber, pageTableEntry* pageTable);



int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        printf("Incorrect command line formatting.\n");
        exit(0);
    }

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        printf("Error opening input file.\n");
        exit(1);
    }

    tlbEntry tlb[TLB_SIZE];
    pageTableEntry pageTable[PAGE_TABLE_SIZE];
    int physicalMemory[PHYSICAL_MEMORY_SIZE];

    int pageNumber, offset, logicalAddress, frameNumber;
    int tlbHits = 0, pageFaults = 0, totalAddresses = 0;

    while (fscanf(inputFile, "%d", &logicalAddress) != EOF) 
    {
        totalAddresses++;
        extractPageAndOffset(logicalAddress, &pageNumber, &offset);

        if(tlbHit(pageNumber, tlb))
        {
            frameNumber = getFrameNumberFromTLB(pageNumber, tlb);
            tlbHits++;
        }

        else
        {
            frameNumber = getFrameNumberFromPageTable(pageNumber, pageTable);

            if (frameNumber == -1)
            {
                handlePageFault(pageNumber, pageTable, physicalMemory);
                pageFaults++;

                frameNumber = getFrameNumberFromPageTable(pageNumber, pageTable);
            }

              updateTLB(pageNumber, frameNumber, tlb);
        }

        if(pageNumberHit(pageNumber, pageTable))
        {
            frameNumber = getFrameNumberFromPageTable(pageNumber, pageTable);
        }

        else
        {
            handlePageFault(pageNumber, pageTable, physicalMemory);
            pageFaults++;

            frameNumber = getFrameNumberFromPageTable(pageNumber, pageTable);
        }

        int physicalAddress = (frameNumber << 8) | offset;
        char signedByte = physicalMemory[physicalAddress];

        printf("Logical Address: %d, Physical Address: %d, Value: %d\n", logicalAddress, physicalAddress, signedByte);

        if (tlbHits != 0)
        {
            updateTLB(pageNumber, frameNumber, tlb);
        }

        updatePageTable(pageNumber, frameNumber, pageTable);
    }

    if (totalAddresses != 0)
    {
        double pageFaultRate = (double) pageFaults / totalAddresses * 100;
        double tlbHitRate = (double) tlbHits / totalAddresses * 100; 
    }

    fclose(inputFile);
    return 0;
}



void extractPageAndOffset(int logicalAddress, int* pageNumber, int* offset)
{
    *pageNumber = (logicalAddress >> 8) & 0xFF;
    *offset = logicalAddress & 0xFF;
}

int tlbHit(int pageNumber, tlbEntry* tlb)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i].pageNumber == pageNumber)
        {
            return 1;
        }
    }

    return 0;
}

int getFrameNumberFromTLB(int pageNumber, tlbEntry* tlb) 
{
    for (int i = 0; i < TLB_SIZE; ++i) 
    {
        if (tlb[i].pageNumber == pageNumber) 
        {
            return tlb[i].frameNumber;
        }
    }

    fprintf("TLB entry not found for page %i\n", pageNumber);
    exit(2);
}

int getFrameNumberFromPageTable(int pageNumber, pageTableEntry* pageTable) 
{
    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) 
    {
        if (pageTable[i].frameNumber == pageNumber && pageTable[i].valid) 
        {
            return pageTable[i].frameNumber;
        }
    }

    return -1;
}

void handlePageFault(int pageNumber, pageTableEntry* pageTable, int* physicalMemory) 
{
    FILE* backingStore = fopen("BACKING_STORE.bin", "rb");
    if (backingStore == NULL) 
    {
        printf("Error opening backing store file.\n");
        exit(4);
    }

    fseek(backingStore, pageNumber *PAGE_SIZE, SEEK_SET);

    int frameNumber;
    for (frameNumber = 0; frameNumber < PHYSICAL_MEMORY_SIZE / FRAME_SIZE; ++frameNumber) 
    {
        if (!pageTable[frameNumber].valid) 
        {
            break;
        }
    }

    fread(&physicalMemory[frameNumber * FRAME_SIZE], 1, PAGE_SIZE, backingStore);

    pageTable[frameNumber].frameNumber = frameNumber;
    pageTable[frameNumber].valid = 1;

    fclose(backingStore);
}

void updateTLB(int pageNumber, int frameNumber, tlbEntry *tlb) 
{
    for (int i = 0; i < TLB_SIZE; ++i) 
    {
        if (tlb[i].pageNumber == pageNumber) 
        {
            tlb[i].frameNumber = frameNumber;
            return;
        }
    }
}

int pageNumberHit(int pageNumber, pageTableEntry* pageTable)
{
    for (int i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        if (pageTable[i].frameNumber == pageNumber)
        {
            return 1;
        }
    }

    return 0;
}

void updatePageTable(int pageNumber, int frameNumber, pageTableEntry* pageTable) 
{
    pageTable[pageNumber].frameNumber = frameNumber;
    pageTable[pageNumber].valid = 1;  
}

