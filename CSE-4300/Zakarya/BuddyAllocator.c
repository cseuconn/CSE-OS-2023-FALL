#include <stdint.h>
#include <windows.h>
#include <stdio.h>
#include <assert.h>

typedef struct
{
    size_t size;    // size = requested + sizeof(Block)
    uint8_t bIsFree : 1;    // 1 for free, 0 for used
}Block;

typedef struct
{
    Block* head;
    Block* tail;
}Buddy_Alloc;

#define DEBUG 1
#define POOL_SIZE 128 //bytes
Buddy_Alloc* memPool;


void InitializeBlock(size_t size)
{
    LPVOID processHeap = HeapAlloc(GetProcessHeap(), 0, size);

    if (processHeap == NULL) 
    {
        fprintf(stderr, "Error allocating initial mempry pool\n");
        abort();
    }

    memPool->head = processHeap;
    memPool->head->size = size;
    memPool->head->bIsFree = 1;

    memPool->tail = ((char*) processHeap) + size;
    memPool->tail->size = 0;
    memPool->tail->bIsFree = 0;
}

size_t AdjustSize(size_t size)
{
    size += sizeof(Block);
    size_t actualSize = 2;
    while (actualSize < size)
    {
        actualSize <<= 1;
    }
    return actualSize;
}

Block* GetBuddyBlock(Block* block)
{
    assert(block != NULL);
    return (Block*) ((char*) block + block->size);
}

//Size received should have already been padded in AdjustSize
Block* GetBestFitBlock(Block* curBlock, size_t size)
{
    assert(curBlock != NULL);
    assert(curBlock >= memPool->head);
    assert(curBlock < memPool->tail && "Allocator is out of space");

    if (DEBUG)
    {
        printf("[BFB]::Looking for block with size: %d\n", size);
        printf("[BFB]::Current Block: %p, Size: %d, IsFree: %d\n", curBlock, curBlock->size, curBlock->bIsFree);
    }

    if (curBlock->bIsFree == 0) return GetBestFitBlock(GetBuddyBlock(curBlock), size);

    size_t nextBlockSize = curBlock->size >> 1;
    // If nextBlockSize is too small, then we found our perfectSize
    if (size > nextBlockSize) 
    {
        if (curBlock->size >= size) return curBlock;
        return GetBestFitBlock(GetBuddyBlock(curBlock), size);
    }

    //Split out block into 2 smaller blocks
    curBlock->size = nextBlockSize;

    Block* nextBlock = ((char*) curBlock) + nextBlockSize;
    nextBlock->size = nextBlockSize;
    nextBlock->bIsFree = 1; 

    return GetBestFitBlock(curBlock, size);
}

void* BuddyMalloc(size_t size)
{
    size_t requested = size;

    assert(memPool->head != NULL);
    assert(memPool->tail != NULL);

    size = AdjustSize(size);
    Block* memoryBlock = GetBestFitBlock(memPool->head, size);

    assert(memoryBlock != NULL);
    memoryBlock->bIsFree = 0;

    if (DEBUG)
    {
        printf("[MALLOC]::Requested Size: %d, Given: %d\n", requested, size);
        printf("[MALLOC]::Block start: %p, user receive: %p\n", memoryBlock, ((char*) memoryBlock) + sizeof(Block));
        printf("\n");
    }

    return ((char*) memoryBlock) + sizeof(Block);
}

void BuddyFree(void* block)
{
    assert(block != NULL);

    Block* memBlock = ((char*) block) - sizeof(Block);
    assert(memBlock->size > sizeof(Block));

    memBlock->bIsFree = 1;

    if (DEBUG)
    {
        printf("[FREE]::Block start: %p, user receive: %p\n", memBlock, block);
    }

    //Check for buddy blocks if same size and combine
    Block* curBlock = memPool->head;
    while (curBlock < memPool->tail)
    {
        Block* buddyBlock = GetBuddyBlock(curBlock);
        if (DEBUG)
        {
            printf("[FREE]::CurBlock: %p, Size: %d, IsFree: %d\n", curBlock, curBlock->size, curBlock->bIsFree);
            printf("[FREE]::BuddyBlock: %p, Size: %d, IsFree: %d\n", buddyBlock, buddyBlock->size, buddyBlock->bIsFree);
            printf("\n");
        }

        if (buddyBlock == memPool->tail) return;

        if (curBlock->bIsFree == 0) 
        {
            curBlock = buddyBlock;
            continue;
        }

        // if (buddyBlock->bIsFree == 1)
        // {
        //     curBlock->size += buddyBlock->size;
        // }
        // else
        // {
        //     curBlock = GetBuddyBlock(buddyBlock);
        // }

        curBlock->size += buddyBlock->bIsFree == 1? buddyBlock->size : 0;
        curBlock = buddyBlock->bIsFree == 0? GetBuddyBlock(buddyBlock) : curBlock;
    }
}

void DebugMem()
{
    printf("\n");
    printf("[DEBUG_MEM]\n");

    size_t memUsed = 0;
    Block* curBlock = memPool->head;
    while (curBlock < memPool->tail)
    {
        if (curBlock->bIsFree == 0) memUsed += curBlock->size;
        curBlock = GetBuddyBlock(curBlock);
    }

    printf("----");
    printf("Memory Used: %d, Memory left: %d\n", memUsed, POOL_SIZE - memUsed);
    printf("\n");
}

int main()
{
    memPool = malloc(sizeof(Buddy_Alloc));
    memPool->head = malloc(sizeof(Block));
    memPool->tail = malloc(sizeof(Block));

    InitializeBlock(POOL_SIZE);
    printf("BLOCK START: %p\n", memPool->head);
    printf("BLOCK END: %p\n", memPool->tail);
    printf("HEAD block size: %d\n", memPool->head->size);
    printf("HEAD block empty: %d\n", memPool->head->bIsFree);

    void* mem1 = BuddyMalloc(20);
    void* mem2 = BuddyMalloc(50);

    BuddyFree(mem2);

    return 0;
}