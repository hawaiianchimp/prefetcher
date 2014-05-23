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
	Request req = _nextReq[pcpointer];
	return req;
}

void Prefetcher::completeRequest(u_int32_t cycle) {
		//_ready = false;
		pcpointer++;
		pcpointer %= 127;
		//_nextReq.addr = _nextReq.addr +32;
	}

void Prefetcher::cpuRequest(Request req) { 
	pcpointer = 0;
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
	if(!req.HitL1)
	{
		for(int i=0; i<8;i++)
		{
			_nextReq[stackpointer+i].addr = req.addr + 32*(i+1);
			stackpointer++;
			stackpointer %= 127;
			_ready = true;
		}
		//_nextReq.addr = req.addr + 32;	
	}

}
