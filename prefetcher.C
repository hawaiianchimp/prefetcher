#include "prefetcher.h"
#include <stdio.h>
#include "mem-sim.h"
Prefetcher::Prefetcher(){
	_ready = false;
	pcpointer = 0;
	stackpointer = 0;
}

bool Prefetcher::hasRequest(u_int32_t cycle) {
	return _ready;
}

Request Prefetcher::getRequest(u_int32_t cycle) {

	for(;_nextReq[pcpointer].addr == 0;pcpointer++);
	printf("_nextReq[%u] = %u\n", pcpointer, _nextReq[pcpointer].addr);
	Request req = _nextReq[pcpointer];
	return req;
}

void Prefetcher::completeRequest(u_int32_t cycle) {
		pcpointer++;
		pcpointer %= 128;
		if(cycle % 4 == 0)
		{
			_ready = false;
		}
		//_nextReq.addr = _nextReq.addr +32;
	}

void Prefetcher::cpuRequest(Request req) { 
	if(false)
	{
		int addr = req.pc & 127;
		pcfrequency[addr] = req.addr;

		printf("addr=%u\n", addr);
		for(int i=0;i<128;i++)
		{
			_nextReq[i].addr = pcfrequency[i];
		}
	}	
	
	if(!req.HitL1 && !req.HitL2 && false)
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
		for(int i=0; i<4;i++)
		{
			_nextReq[stackpointer+i].addr = req.addr + 32*(i+1);
			stackpointer++;
			stackpointer %= 128;
			printf("sp=%u\n", stackpointer);
			_ready = true;
		}
		//_nextReq.addr = req.addr + 32;	
	}

}
