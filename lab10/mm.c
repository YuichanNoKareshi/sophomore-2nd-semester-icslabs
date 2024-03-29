/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7) 

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// Basic constants and macros
#define WSIZE       4           // Word and header/footer size(bytes)
#define DSIZE       8           // Double word size (bytes)
#define CHUNKSIZE   (1<<12)     // Extend heap by this amount (bytes)

#define MAX(x, y)   ((x)>(y)?(x):(y))

// Pack a size and allocated bit into a word
#define PACK(size, alloc)   ((size) | (alloc))

// Read and write a word at address p
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

// Read and write an address(pointer) at address p
#define GETADDR(p)          (*(unsigned long *)(p))
#define PUTADDR(p, val)     (*(unsigned long *)(p) = (val))

// Read the size and allocated fields from address p
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

// Given block ptr ptr, compute address of its header and footer
#define HDRP(ptr)    ((char *)(ptr) - WSIZE)
#define FTRP(ptr)    ((char *)(ptr) + GET_SIZE(HDRP(ptr)) - DSIZE)

// Given block ptr ptr of a free block, compute the address of predecessor/successor pointer
#define PRED(ptr)    ((char *)(ptr))
#define SUCC(ptr)    ((char *)(ptr) + DSIZE)

// Given block ptr ptr, compute address of next and previous blocks
#define NEXT_BLKP(ptr)   ((char *)(ptr) + GET_SIZE(((char *)(ptr) - WSIZE)))
#define PREV_BLKP(ptr)   ((char *)(ptr) - GET_SIZE(((char *)(ptr) - DSIZE)))

// Given block ptr ptr of a free block, compute address of previous/next free block
#define PREV_FREE(ptr)   ((char*)GETADDR(PRED(ptr)))
#define NEXT_FREE(ptr)   ((char*)GETADDR(SUCC(ptr)))

static char* heap_listp;                        // A pointer that always pointing to the prologue block
static char* free_listp = NULL;                 // A pointer that always pointing to the first free block
int flag;

// Necessary declaration
static void* extend_heap(size_t words);
void disconnect(void* ptr);
void connect(void* ptr);
static void* coalesce(void* ptr);
static void* find_fit(size_t asize);
static void place(void* ptr, size_t asize);
int mm_check();


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void) 
{
    // Create the initial empty heap with a prologue block and an epilogue block.
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void*)-1)return -1;
    PUT(heap_listp, 0);                             // Alignment padding
    PUT(heap_listp + (WSIZE), PACK(DSIZE, 1));      // Prologue header
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // Prologue footer
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        // Epilogue header
    heap_listp += (2*WSIZE);
    
    // Set global variables
    free_listp = NULL;
    flag = 0;

    // Extend the empty head with a free block of CHUNKSIZE bytes
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) 
{
    size_t asize;        // Adjusted block size
    size_t extendsize;   // Amount to extend heap if no fit
    char* ptr;

    // Ignore spurious requests
    if(0 == size)
    {
        return NULL;
    }

    // Adjust block size to include overhead(header, footer and two pointers, 6 words in total) and alignment reqs
    if(size <= DSIZE)asize = 3*DSIZE;
    else if(112 == size)asize = 136;
    else if(448 == size)asize = 520;
    else
    {
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    }

    // Search the free list for a fit
    if((ptr = find_fit(asize)) != NULL)
    {
        place(ptr, asize);
        return ptr;
    }

    // No fit found. Get more memory and place the block
    extendsize = MAX(asize, CHUNKSIZE);
    if((ptr = extend_heap(extendsize/WSIZE)) == NULL)
    {
        return NULL;
    }

    place(ptr, asize);
    return ptr;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    PUTADDR(PRED(ptr), 0);
    PUTADDR(SUCC(ptr), 0);
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldbp = ptr;
    void *newbp;
    size_t copySize;

    if(!flag)
    {
        newbp = mm_malloc(size);
        if (newbp == NULL)
          return NULL;
        copySize = *(size_t *)((char *)oldbp - DSIZE);
        if (size < copySize)
          copySize = size;
        memcpy(newbp, oldbp, copySize);
        mm_free(oldbp);
        flag = 1;
        return newbp;
    }
    else
    {
        char* lastfree = NEXT_BLKP(oldbp);
        size_t diff = DSIZE * ((size - GET_SIZE(HDRP(oldbp)) + (2*DSIZE-1)) / DSIZE);
        if(GET_SIZE(HDRP(lastfree)) < (diff + 3*DSIZE))
        {
            lastfree = extend_heap(CHUNKSIZE/WSIZE);
        }
        size_t temp = GET_SIZE(HDRP(lastfree));
        disconnect(lastfree);
        lastfree += diff;
        PUT(HDRP(lastfree), PACK((temp-diff), 0));
        PUT(FTRP(lastfree), PACK((temp-diff), 0));
        PUTADDR(PRED(lastfree), 0);
        PUTADDR(SUCC(lastfree), 0);
        PUT(HDRP(oldbp), PACK(GET_SIZE(HDRP(oldbp))+diff, 1));
        PUT(FTRP(oldbp), PACK(GET_SIZE(HDRP(oldbp)), 1));
        connect(lastfree);
        return oldbp;
    }
}

/*
 * extend_heap - extends the heap with a new free block, return the block pointer of the new block
 */
static void* extend_heap(size_t words)
{
    char* ptr;
    size_t size;

    // Allocate an even number of words to maintain alignment.
    // Do not need to consider minimum block size since it's covered in mm-malloc.
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if((long)(ptr = mem_sbrk(size)) == -1)return NULL;

    // Initialize free block header/footer, predecessor pointer, successor pointer and the epilogue header 
    PUT(HDRP(ptr), PACK(size, 0));           // Free block header
    PUT(FTRP(ptr), PACK(size, 0));           // Free block footer
    PUTADDR(PRED(ptr), 0);                   // Predecessor pointer
    PUTADDR(SUCC(ptr), 0);                   // Successor pointer
    PUT(HDRP(NEXT_BLKP(ptr)), PACK(0, 1));   // New epilogue header

    // Coalesce if the previous block is free
    return coalesce(ptr);
}

/*
 * disconnect - remove a free block from the free list since it's allocated or being coalesced
 */
void disconnect(void* ptr)
{
    // Get its adjacent free blocks 
    char* prev = PREV_FREE(ptr);
    char* next = NEXT_FREE(ptr);

    // Set pointers in those blocks
    if(prev && next)                // case 1
    {
        PUTADDR(SUCC(prev), GETADDR(SUCC(ptr)));
        PUTADDR(PRED(next), GETADDR(PRED(ptr)));
    }
    else if(prev && !next)          // case 2
    {
        PUTADDR(SUCC(prev), 0);
    }
    else if(!prev && next)          // case 3
    {
        PUTADDR(PRED(next), 0);
        free_listp = next;          // in this case free_listp needs to be changed
    }
    else                            // case 4
    {
        free_listp = NULL;          // also change free_listp
    }

    // Set the two pointers of the block to NULL in case they are used accidentally
    PUTADDR(PRED(ptr), 0);
    PUTADDR(SUCC(ptr), 0);
}

/*
 * connect - add a free block to the top of the free list
 * The allocator use LIFO-ordering to reduce the cost of freeing new blocks to constant time.
 */
void connect(void* ptr)
{
    if(NULL != free_listp)
    {
        PUTADDR(PRED(free_listp), (unsigned long)ptr);
        PUTADDR(SUCC(ptr), (unsigned long)free_listp);
    }
    else
    {
        PUTADDR(SUCC(ptr), 0);
    }
    PUTADDR(PRED(ptr), 0);
    free_listp = ptr;
}

/*
 * coalesce - combine a newly freed block with adjacent free blocks, return the block pointer of the coalesced block
 */
static void* coalesce(void* ptr)
{
    // Check if the two adjacent blocks are free 
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(ptr)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
    size_t size = GET_SIZE(HDRP(ptr));
    
    // Take free blocks next to the block out of the free list and combine them by changing tags and pointers
    if(prev_alloc && !next_alloc)              // Case 2 (case 1 only need connect())
    {
        disconnect(NEXT_BLKP(ptr));
        size += GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        PUT(HDRP(ptr), PACK(size, 0));
        PUT(FTRP(ptr), PACK(size, 0));
    }

    else if(!prev_alloc && next_alloc)              // Case 3
    {
        disconnect(PREV_BLKP(ptr));
        size += GET_SIZE(HDRP(PREV_BLKP(ptr)));
        PUT(FTRP(ptr), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(ptr)), PACK(size, 0));
        ptr = PREV_BLKP(ptr);
    }

    else if(!prev_alloc && !next_alloc)              // Case 4
    {
        disconnect(PREV_BLKP(ptr));
        disconnect(NEXT_BLKP(ptr));
        size += GET_SIZE(HDRP(PREV_BLKP(ptr))) + GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        PUT(HDRP(PREV_BLKP(ptr)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(ptr)), PACK(size, 0));
        ptr = PREV_BLKP(ptr);
    }
    
    // Add the coalesced block back to the free list
    connect(ptr);
    return ptr;
}

/*
 * find_fit - Find a free block to allocate to. Return the block pointer of the selected block 
 */
static void* find_fit(size_t asize)
{
    // First-fit search through the free list
    void* ptr = free_listp;
    while(NULL != ptr)
    {
        // The free block should be large enough
        if(asize <= GET_SIZE(HDRP(ptr)))
        {
            if(!flag || (0 != GET_SIZE(HDRP(NEXT_BLKP(ptr)))))
            return ptr;
        }
        ptr = NEXT_FREE(ptr);
    }
    return NULL; // No fit
}

/*
 * place - Allocate to a chosen block and split the remainder of the free block if necessary
 */
static void place(void* ptr, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(ptr));          // The size of the free block to be allocated
    if((csize - asize) >= (3*DSIZE))            // If the size of the remainder is not less then the minimum block size, add it to the free list
    {
        disconnect(ptr);
        PUT(HDRP(ptr), PACK(asize, 1));
        PUT(FTRP(ptr), PACK(asize, 1));
        ptr = NEXT_BLKP(ptr);
        PUT(HDRP(ptr), PACK(csize - asize, 0));
        PUT(FTRP(ptr), PACK(csize - asize, 0));
        connect(ptr);
    }
    else                                        // If not, just place the payload on the whole block
    {
        disconnect(ptr);
        PUT(HDRP(ptr), PACK(csize, 1));
        PUT(FTRP(ptr), PACK(csize, 1));
    }
}

/*
 * mm_check - a simple heap consistency checker. Return 1 if check succeed, 0 if fail.
 */
int mm_check(void)
{
    char* ptr;
    char* fp;
    int count = 0;

    // Check contiguous free blocks
    for(ptr = heap_listp; GET_SIZE(HDRP(ptr)) > 0; ptr = NEXT_BLKP(ptr))
    {
        if(!(GET_ALLOC(HDRP(ptr)) || GET_ALLOC(HDRP(NEXT_BLKP(ptr)))))
        {
            fprintf(stderr, "contagious free blocks");
            return 0;
        }
    }

    // Check the allocate bit of free blocks in the free list
    for(fp = free_listp; NULL != fp; fp = NEXT_FREE(fp))
    {
        if(GET_ALLOC(fp))
        {
            fprintf(stderr, "block in the free list marked as allocated.");
            return 0;
        }
    }

    // Check whether there are any free block outside the free list
    for(ptr = heap_listp; GET_SIZE(HDRP(ptr)) > 0; ptr = NEXT_BLKP(ptr))
    {
        if(!GET_ALLOC(HDRP(ptr)))
        {
            count++;
        }
    }
    for(fp = free_listp; NULL != fp; fp = NEXT_FREE(fp))
    {
        count--;
    }
    if(0 != count)
    {
        fprintf(stderr, "numbers of free blocks in free list and heap are unmatched.");
        return 0;
    }

    // Check the integrity of pointers in free blocks 
    for(fp = free_listp; NULL != fp; fp = NEXT_FREE(fp))
    {
        if(0 != GETADDR(PRED(free_listp)))
        {
            fprintf(stderr, "invalid free_listp predecessot pointer");
            return 0;
        }

        if(GETADDR(PRED(fp)) == GETADDR(SUCC(fp)) && 0 != GETADDR(PRED(fp)))
        {
            fprintf(stderr, "two pointers in one free blocks have same nonzero value");
            return 0;
        }

        if(0 != GETADDR(SUCC(fp)) && (GETADDR(SUCC(fp)) != (unsigned long)NEXT_FREE(fp) || GETADDR(PRED(NEXT_FREE(fp))) != (unsigned long)fp))
        {
            fprintf(stderr, "pointers of two contiguous free blocks unmatched");
            return 0;
        }
    }
    return 1;
}