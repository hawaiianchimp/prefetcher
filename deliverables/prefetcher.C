#include "prefetcher.h"
#include <stdio.h>
#include "mem-sim.h"
Prefetcher::Prefetcher(){
	_ready = false;
}

bool Prefetcher::hasRequest(u_int32_t cycle) {
	return _ready;
}

Request Prefetcher::getRequest(u_int32_t cycle) {
	return _nextReq;
}

void Prefetcher::completeRequest(u_int32_t cycle) {
	if (priority ==0){


		_ready = false;
	} else
	{
		_ready = true;
		_nextReq.addr = _nextReq.addr +32;
		priority = 0;
	}
	}

void Prefetcher::cpuRequest(Request req) { 
	priority=0;
	if(!req.HitL1 && !req.HitL2)
	{

		printf("-----------------------------------\n");
		printf("addr=%u\n", req.addr);
		printf("pc=%u\n", req.pc);
		printf("load=%u\n", req.load);
		printf("fromCPU=%u\n", req.fromCPU);
		printf("issueAt=%u\n", req.issuedAt);
		printf("HitL1=%u\n", req.HitL1);
		printf("HitL2=%u\n", req.HitL2);
	}	
	if(!_ready && !req.HitL1)
	{
		priority = 1;
		_nextReq.addr = req.addr + 32;
		_ready = true;		
	}

}
