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
	reqcapacity = 2;
    prefetchtrue = 0;
    prefetchaddr = 0;
}

bool Prefetcher::hasRequest(u_int32_t cycle) {
    //printf("hasrequest = %d\n", reqsize);
	if (reqsize > 0)
		return true;
	else return false;
}

Request Prefetcher::getRequest(u_int32_t cycle) {
    printf("getting request\n");
	return req[reqhead];
}

void Prefetcher::completeRequest(u_int32_t cycle) {
    printf("completing request\n");
    reqhead = (reqhead + 1) % reqcapacity;
    reqsize = reqsize - 1;
    
}

void Prefetcher::cpuRequest(Request req) {
    printf("hit l1? %d\n", req.HitL1);
	if (!req.HitL1)
    {
        prefetchtrue = 1;
        prefetchaddr = req.addr;
    }
    else
    {
        if (prefetchtrue == 1)
            addToHash(prefetchaddr);
        prefetchtrue = 0;
    }
}

void Prefetcher::addToHash(u_int32_t mem)
{
    //printf("adding to hash\n");
	u_int32_t index, tag, terminus, oldtag;
    
	index = mem & MEM_INDEX;
	index = index >> OFFSET_SIZE;
	tag = mem & MEM_TAG;
	tag = tag >> INDEX_SIZE;
	tag = tag >> OFFSET_SIZE;
   // printf("index: %d\n", index);
   // printf("hashprev: %d\n", hashprev);
   // printf("tag: %d\n", tag);
    
	if (hashprev >= 0)  //hash is filled with at least one value
	{
		
		if (index == hashprev) //replace tag with new tag
		{
			hashindex[hashprev] = hashindex[hashprev] & HASH_TERMINUS;
			hashindex[hashprev] = hashindex[hashprev] | tag;
		}
        
		else
		{
			hashindex[hashprev] = hashindex[hashprev] & HASH_TAG_TERM;
			hashindex[hashprev] = hashindex[hashprev] << INDEX_SIZE;
			hashindex[hashprev] = hashindex[hashprev] | index;
            
			terminus = hashindex[index] & HASH_TERMINUS;
			oldtag = hashindex[index] & HASH_TAG;
            oldtag = oldtag >> INDEX_SIZE;
          //  printf("oldtag: %d\n", oldtag);
			if (terminus)
			{
				hashindex[index] = hashindex[index] & HASH_TERMINUS;
				hashindex[index] = hashindex[index] | tag;
				hashprev = index;
			}
			else if (tag != oldtag)
			{
				hashindex[index] = hashindex[index] & HASH_TERMINUS;
				hashindex[index] = hashindex[index] | tag;
				hashprev = index;
			}
            
			else
			{
                printf("prefetch occurring!\n");
                prefetch(index);
			}
		}
	}
    
	else
	{
		hashindex[index] = hashindex[index] & HASH_TERMINUS;
		hashindex[index] = hashindex[index] | tag;
		hashprev = index;
	}
}

void Prefetcher::prefetch(u_int32_t index)
{
  //  printf("prefetching\n");
	u_int32_t address, prev;
    prev = index;
	u_int32_t terminus = 0;
    u_int32_t x = reqcapacity - reqsize;
	for (int i = 0; i < x && terminus == 0; i++)
	{
      //  printf("i: %d\n", i);
      //  printf("reqcapacity: %d\n", reqcapacity);
       // printf("reqsize: %d\n", reqsize);
        
		reqtail = (reqtail + 1) % reqcapacity;
		reqsize = reqsize + 1;
		index = hashindex[prev] & HASH_INDEX;
		terminus = hashindex[index] & HASH_TERMINUS;
      //  printf("terminus: %d\n", terminus);
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
        
		req[reqtail].addr = address;
		prev = index;
	}
}
