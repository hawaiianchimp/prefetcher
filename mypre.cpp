#include "prefetcher.h"
#include <stdio.h>
#include "mem-sim.h"

#define MEM_TAG 0xFFFFC000
#define MEM_INDEX 0x00003FF0
#define MEM_OFFSET 0X0000000F
#define HASH_TAG_TERM 0x0003FFFF
#define HASH_TAG 0x0FFFFC00
#define HASH_INDEX 0x000003FF
#define HASH_TERMINUS 0xF0000000
#define TAG_SIZE 18
#define INDEX_SIZE 10
#define OFFSET_SIZE 4
#define CLEAR 0

Prefetcher::Prefetcher(){
	hashprev = -1;
	reqhead = 0;
	reqtail = 0;
	reqsize = 0;
	reqcapacity = 10;
}

bool Prefetcher::hasRequest(u_int32_t cycle) {
	if (reqsize > 0)
		return TRUE;
	else return FALSE;
}

Request Prefetcher::getRequest(u_int32_t cycle) {
	return req[head];
}

void Prefetcher::completeRequest(u_int32_t cycle) {
		req[head].addr = 0;
		head = (head + 1) % capacity;
		reqsize = reqsize - 1;
	}

void Prefetcher::cpuRequest(Request req) { 
	if (!req.HitL1)
		addToHash(req.addr);
}

void Prefetcher::addToHash(u_int32_t mem)
{
	u_int32_t index, tag, terminus, oldtag;
    
	index = mem & MEM_INDEX;
	index = index >> OFFSET_SIZE;
	tag = mem & MEM_TAG;
	tag = tag >> INDEX_SIZE;
	tag = tag >> OFFSET_SIZE;
    
	if (prev >= 0)  //hash is filled with at least one value
	{
		
		if (index == prev) //replace tag with new tag
		{
			hashindex[prev] = hashindex[prev] & ~(HASH_TAG_TERM);
			hashindex[prev] = hashindex[prev] | tag;
		}

		else
		{
			hashindex[prev] = hashindex[prev] & REMOVE_TERMINUS;
			hashindex[prev] = hashindex[prev] << INDEX_SIZE;
			hashindex[prev] = hashindex[prev] | index;

			terminus = hashindex[index] & EXTRACT_TERMINUS;
			oldtag = hashindex[index] & EXTRACT_TAG_HASH;
			if (terminus)
			{
				hashindex[index] = hashindex[index] & EXTRACT_TERMINUS;
				hashindex[index] = hashindex[index] | tag;
				prev = index;
			}
			else if (tag != oldtag)
			{
				hashindex[index] = hashindex[index] & CLEAR;
				hashindex[index] = hashindex[index] | EXTRACT_TERMINUS;
				hashindex[index] = hashindex[index] | tag;
				prev = index;
			}
			else
			{
				prefetch(index);
			}
		}
	}
    
	else
	{
		hashindex[index] = hashindex[index] & CLEAR;
		hashindex[index] = hashindex[index] | HASH_TERMINUS;
		hashindex[index] = hashindex[index] | tag;
		prev = index; 
	}
}

void Prefetcher::prefetch(int index)
{
	int address, prev;
	int terminus = 0;
	for (int i = 0; i < (capacity - reqsize) && terminus == 0; i++)
	{
		tail = (tail + 1) % capacity;
		reqsize = reqsize + 1;
		index = hashindex[prev] & HASH_INDEX;
		terminus = hashindex[index] & HASH_TERMINUS;
		if (terminus == 0)
		{
			address = hashindex[index] & HASH_TAG;
			address = address | index;
			address = address << OFFSET_SIZE;
		}
		else
		{
			address = hashindex[index] & HASH_TAG_TERM;
			address = address << INDEX_SIZE;
			address = address | index;
			address = address << OFFSET_SIZE;
		}

		reqs[tail].addr = address;
		prev = index;
	}
}
