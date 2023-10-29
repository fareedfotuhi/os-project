#include "my_alloc.h"
#include "limits.h"
#include "math.h"

// #define max(i,j) i > j ? i : j
//-----------------------------------------------------------------------------------------

list blocks_list = {NULL, NULL};

unsigned int MAXAlloc = UINT_MAX;

unsigned int MINAlloc = 0;

int modifiedMem = 0;

int Alloc_Algorithm = FIRSTFIT_ALG;
//-----------------------------------------------------------------------------------------

void nofrag(block_meta_data *block, block_meta_data *prev);

block_meta_data *find_block(size_t size);

int extendHeap(size_t size);

void splitBlock(block_meta_data *block, size_t requestedSize);

void mergeBlocks(block_meta_data *target, block_meta_data *block);

int CheckLimits(size_t size);

void FillBlock(block_meta_data *block, char fill);
//-----------------------------------------------------------------------------------------

block_meta_data *find_block(size_t size)
{
    block_meta_data *current = blocks_list.base;
    while (current != NULL && (!current->isFree || current->size < size))
    {
        current = current->next;
    }
    return current;
}

int extendHeap(size_t size)
{
    void *mem;
    if ((mem = sbrk(BLOCK_META_DATA_SIZE + size)) == (void *)-1)
        return 0;

    block_meta_data *newBlock = (block_meta_data *)mem;
    newBlock->size = size;
    newBlock->next = NULL;
    newBlock->isFree = 0;
    if (blocks_list.end == NULL)
        blocks_list.base = blocks_list.end = newBlock;
    else
    {
        blocks_list.end->next = newBlock;
        blocks_list.end = newBlock;
    }
    return 1;
}

void splitBlock(block_meta_data *block, size_t requestedSize)
{
    size_t Allocated_Mem = requestedSize;
    char *bytePointer = (char *)block;
    block_meta_data *nextBlock = block->next;
    // if (Alloc_Algorithm == BUDDY_ALG)
    // {//{TODO}
    //     size_t blockSize = block->size ;
    //     while (blockSize >= requestedSize)
    //     {
    //         blockSize >>= 1 ;
    //     }
    //     blockSize <<= 1 ;
    //     Allocated_Mem = blockSize ;
    // }
    block->next = (block_meta_data *)(bytePointer + BLOCK_META_DATA_SIZE + Allocated_Mem);
    block->next->size = block->size - Allocated_Mem - BLOCK_META_DATA_SIZE;
    block->size = Allocated_Mem;
    block->next->next = nextBlock;
    block->next->isFree = 1;
    block->isFree = 0;
    if (nextBlock == NULL)
        blocks_list.end = block->next;
}

// void* calloc_v2(size_t numberOfElements, size_t sizeOfEach)
// {
//     void* mem = my_malloc(numberOfElements * sizeOfEach);
//     size_t bytes = align4(numberOfElements * sizeOfEach);
//     for(int i=0; i<bytes; i++) *(((char*)mem) + i) = 0;
//     return mem;
// }

void mergeBlocks(block_meta_data *target, block_meta_data *block)
{
    target->next = block->next;
    target->size = target->size + BLOCK_META_DATA_SIZE + block->size;
}

void nofrag(block_meta_data *block, block_meta_data *prev)
{
    block_meta_data *next = block->next;
    if (prev != NULL && prev->isFree)
    {
        mergeBlocks(prev, block);
        block = prev;
    }
    if (next != NULL && next->isFree)
        mergeBlocks(block, next); // merge with next
}

int CheckLimits(size_t size)
{
    int res = 0;
    if (size <= MAXAlloc && size >= MINAlloc)
    {
        res = 1;
    }
    else
        printf("memory allocation is not legal due to limit violation.");

    return res;
}
void FillBlock(block_meta_data *block, char fill)
{
    for (int i = 0; i < block->size; i++)
        *(((char *)block + BLOCK_META_DATA_SIZE) + i) = fill;
}

//-----------------------------------------------------------------------------------------
void SetAllocAlgorithm(int ALG)
{
    if (modifiedMem != MODIFIED)
    {
        Alloc_Algorithm = ALG;
        printf("Allocation Algorithm is set to %d.\n", ALG);
    }
}
void my_free(void *mem)
{
    modifiedMem = MODIFIED;
    if (mem == NULL)
        return;
    block_meta_data *block = (block_meta_data *)(((char *)mem) - BLOCK_META_DATA_SIZE);
    int isAllocated = 0;
    block_meta_data *current = blocks_list.base, *prev = NULL;
    while (current != NULL)
    {
        if (block == current)
        {
            isAllocated = 1;
            break;
        }
        prev = current;
        current = current->next;
    }
    if (isAllocated)
    {
        if (!block->isFree)
        {
            block->isFree = 1;
            nofrag(block, prev);
        }
        else
            printf("Info : the block is already my_free and available for storage.\n");
    }
    else
    {
        fprintf(stderr, "error to my_free a block that is not previously allocated by my_malloc.\n");
    }
}

void setMinMax(unsigned int min, unsigned int max)
{
    if (min > 0)
    {
        MINAlloc = min;
    }
    if (max >= 0)
    {
        MAXAlloc = max;
    }
}
void *my_malloc(size_t size, char fill)
{
    modifiedMem = MODIFIED;
    if (!CheckLimits(size))
        return NULL;
    size_t alignedSize = align4(size);
    block_meta_data *chunk = find_block(alignedSize);
    if (chunk != NULL)
    {

        int availableSpace = (int)chunk->size - (int)alignedSize - BLOCK_META_DATA_SIZE;
        if (availableSpace >= 4)
        {
            splitBlock(chunk, alignedSize);
        }
        // chunk->isFree = 0;
        FillBlock(chunk, fill);
        return ((void *)chunk) + BLOCK_META_DATA_SIZE;
    }
    else
    {
        if (!extendHeap(alignedSize))
        {
            fprintf(stderr, "Error while trying to extend the heap.\n");
        }
        FillBlock(blocks_list.end, fill);
        return ((void *)blocks_list.end) + BLOCK_META_DATA_SIZE;
    }
}
void show_stats()
{
    block_meta_data *current = blocks_list.base;
    int i = 0;
    size_t allocated = 0, free = 0;
    size_t *breakp = (size_t *)sbrk(0);
    printf("allocated blocks : \n");
    while (current != NULL)
    {
        if (current->isFree == 0)
        {
            printf("block %d --> start =  %p , end = %p , size = %ld\n", i, (void *)current + BLOCK_META_DATA_SIZE, (void *)current + BLOCK_META_DATA_SIZE + current->size, current->size);
            allocated += current->size;
        }
        i++;
        current = current->next;
    }
    if (allocated == 0)
        printf("None\n");
    int j = 0;
    printf("free blocks : \n");
    current = blocks_list.base;
    while (current != NULL)
    {
        if (current->isFree == 1)
        {
            printf("block %d --> start =  %p , end = %p , size = %ld\n", j, (void *)current + BLOCK_META_DATA_SIZE, (void *)current + BLOCK_META_DATA_SIZE + current->size, current->size);
            free += current->size;
        }
        j++;
        current = current->next;
    }
    if (free == 0)
        printf("None\n");
    printf("total allocated : %ld , total free : %ld , the Break pointer diffrence : %p\n", allocated, free, (size_t *)breakp - (allocated + free));
    printf("---------------------------------------------------------------------------------------------\n");

    // while (current != NULL)
    // {
    //     printf("block %d at %p :\n", i++, current);
    //     printf("\tsize = %lu | isFree = %d | next = %p\n", current->size, current->isFree, current->next);
    //     if (current->isFree)
    //         free += current->size;
    //     else
    //         allocated += current->size;
    //     current = current->next;
    // }
}
void *my_realloc(void *ptr, size_t size, char fill)
{
    modifiedMem = MODIFIED;
    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }
    else if (size > 0 && ptr == NULL)
        return (my_malloc(size, fill));
    else
    {
        if (!CheckLimits(size))
            return NULL;
        block_meta_data *block = (block_meta_data *)(((char *)ptr) - BLOCK_META_DATA_SIZE);
        size_t alignedSize = align4(size);
        if (block->size > (int)alignedSize + BLOCK_META_DATA_SIZE + 4)
        {
            splitBlock(block, alignedSize);
            // block->isFree = 0;
            FillBlock(block, fill);
            return ((void *)block) + BLOCK_META_DATA_SIZE;
        }
        else
        {
            if (block->next->isFree == 1 && (block->next->size + block->size) >= (alignedSize))
            {
                mergeBlocks(block, block->next);
                if (block->size > (int)alignedSize + BLOCK_META_DATA_SIZE + 4)
                    splitBlock(block, alignedSize);
                // block->isFree = 0;
                FillBlock(block, fill);
                return ((void *)block) + BLOCK_META_DATA_SIZE;
            }
            else
            {
                block_meta_data *foundBlock = find_block(alignedSize);
                if (foundBlock != NULL)
                {
                    int availableSpace = (int)foundBlock->size - (int)alignedSize - BLOCK_META_DATA_SIZE;
                    if (availableSpace >= 4)
                    {
                        splitBlock(foundBlock, alignedSize);
                    }
                    foundBlock->isFree = 0;
                    // FillBlock(foundBlock, fill);
                    // TODO
                    memcpy((char*)foundBlock + BLOCK_META_DATA_SIZE,(char *)block+ BLOCK_META_DATA_SIZE,block->size);
                    return ((void *)foundBlock) + BLOCK_META_DATA_SIZE;
                }
                else
                    return NULL ;
            }
        }
    }
}