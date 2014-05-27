#ifndef PREFETCHER_H
#define PREFETCHER_H
#define SIZE 4

#include <sys/types.h>
#include "mem-sim.h"

struct Request;

class Prefetcher {
	private:
        u_int32_t hashindex[1024];
        Request req[2];
    int hashprev;
    u_int32_t reqhead;
    u_int32_t reqtail;
    u_int32_t reqsize;
    u_int32_t reqcapacity;
    u_int32_t prefetchaddr;
    u_int32_t prefetchtrue;
    

	
	public:
		Prefetcher();
		// should return true if a request is ready for this cycle
		bool hasRequest(u_int32_t cycle);
	
		// request a desired address be brought in
		Request getRequest(u_int32_t cycle);
	
		// this function is called whenever the last prefetcher request was successfully sent to the L2
		void completeRequest(u_int32_t cycle);
	
		/*
		 * This function is called whenever the CPU references memory.
		 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
		 */
		void cpuRequest(Request req);
        void addToHash(u_int32_t mem);
    void prefetch(u_int32_t index);
};

#endif
