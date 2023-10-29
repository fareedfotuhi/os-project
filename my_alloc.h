#ifndef ALLOC_H
#define ALLOC_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define BUDDY_ALG 1
#define FIRSTFIT_ALG 0
#define MODIFIED 1
#define align4(x) ((((x - 1) >> 2) << 2) + 4)
#define BLOCK_META_DATA_SIZE sizeof(block_meta_data)

struct block_meta_data
{
    size_t size;
    struct block_meta_data *next;
    int isFree;
};
typedef struct block_meta_data block_meta_data;

struct list
{
    block_meta_data *base;
    block_meta_data *end;
};
typedef struct list list;

void *my_malloc(size_t size,char fill);

void my_free(void *mem);

void *my_realloc(void *ptr, size_t size,char fill);

void show_stats();

void setMinMax(unsigned int min, unsigned int max);

void SetAllocAlgorithm(int ALG);
// void* calloc_v2(size_t, size_t);

#endif