/**
 * @file nvram.c
 * @provides nvramInit, nvramGet
 * Functions to access the nvram settings from kernel space.
 *
 * $Id: nvram.c 2218 2010-06-14 23:35:55Z brylow $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>
#include <string.h>

int32	flash_size = -1;	/* size of flash given by boot leader	*/

struct nvram_header *nvram_header = NULL;
struct nvram_tuple *nvram_tuples[NVRAM_NHASH];

static uint32 nvramHash(char *name);
static void nvramInsert(struct nvram_tuple *tuple);

/**
 * Initialize the nvram variable structures for editing
 * @return OK on success, SYSERR on failure
 */
devcall nvramInit(void)
{
    uint32 nvbase, offset, index, pair_len;
    uint32 nvram_length, size;
    char *pair;
    struct nvram_tuple *tuple;

    /* check if we already have initialized nvram */
    if (nvram_header != NULL && NVRAM_MAGIC == nvram_header->magic)
    {
        return OK;
    }

    /* flash size passed in by loader in megabytes */
    if (flash_size < 1024)
    {
	    flash_size = flash_size << 20;
    }

    /* zero out nvram_tuples pointers */
    for (index = 0; index < NVRAM_NHASH; index++)
    {
        nvram_tuples[index] = NULL;
    }

    /* Scan flash at for NVRAM magic number */
    nvbase = FLASH_BASE + flash_size - NVRAM_SIZE;
    while (nvbase > FLASH_BASE)
    {
        if (NVRAM_MAGIC == *((uint32 *)nvbase))
            break;
        nvbase -= NVRAM_SIZE;
    }
    offset = 0;

    /* find the head for data */
    nvram_header =
        (struct nvram_header *)getmem(sizeof(struct nvram_header));
    memcpy(nvram_header, (void *)nvbase, sizeof(struct nvram_header));
    if (nvram_header->magic != NVRAM_MAGIC)
    {
        return SYSERR;
    }

    offset += sizeof(struct nvram_header);

    /* loop through nvram variables and add to array */
    nvram_length = nvram_header->length;

    nvram_header->length = sizeof(struct nvram_header);

    while (offset < nvram_length)
    {
        /* get the length of a string (name=value\0) */
        pair = (char *)(nvbase + offset);
        pair_len = strnlen(pair, nvram_length - offset);

        /* set offset to next string */
        offset += pair_len + 1;

        if (pair_len <= 0)
        {
            continue;
        }

        /* allocate memory to store tuple */
        size = sizeof(struct nvram_tuple) + pair_len;
        tuple = (struct nvram_tuple *)getmem(size);

        /* store tuple */
        memcpy(tuple->pair, pair, pair_len);
        memcpy(tuple->pair + pair_len, "\0", 1);


        nvramInsert(tuple);
    }

    return OK;
}

/**
 * Insert a tuple into the nvram_tuples table
 * @param *tuple pointer to tuple to add (should be memory address)
 * @return OK on success
 */
static void nvramInsert(struct nvram_tuple *tuple)
{
    uint32 index;
    struct nvram_tuple *curr;

    /* hash into table */
    index = nvramHash(tuple->pair);
    curr = nvram_tuples[index];

    /* search for an open position */
    while (curr != NULL && curr->next != NULL)
    {
        curr = curr->next;
    }

    /* fill the open position */
    if (NULL == curr)
    {
        curr = tuple;
        curr->next = NULL;
        nvram_tuples[index] = curr;
    }
    else
    {
        curr->next = tuple;
        tuple->next = NULL;
    }

    nvram_header->length += (strnlen(tuple->pair, NVRAM_STRMAX) + 1);
}

/**
 * Find the value of a variable.
 * @param *name name of variable to find
 * @return pointer to requested tuple struct
 */
char *nvramGet(char *name)
{
    struct nvram_tuple *tuple;
    uint32 hash;

    if (OK != nvramInit() || NULL == name)
    {
        return NULL;
    }

    /* hash the name */
    hash = nvramHash(name);

    /* iterate until name == name */
    tuple = nvram_tuples[hash];
    while ((tuple != NULL) &&
           (strncmp(name, tuple->pair, strnlen(name, NVRAM_STRMAX)) != 0))
    {
        tuple = tuple->next;
    }

    /* make sure a valid string was found, if not return NULL */
    if (NULL == tuple)
    {
        return NULL;
    }

    /* return pointer to start of value name=value pair */
    return tuple->pair + strnlen(name, NVRAM_STRMAX) + 1;
}

/**
 * Given an input string, this will calculate a simple hash between 0 and
 * NVRAM_NHASH-1 (inclusive).
 * @param *p null or '=' terminated string
 * @return value of hashed string
 */
static uint32 nvramHash(char *p)
{
    uint32 sum = 0;
    while (*p && *p != '=')
    {
        sum = 13 * sum + *p++;
    }
    return sum % NVRAM_NHASH;
}
