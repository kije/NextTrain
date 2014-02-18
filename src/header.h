#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
	
#include <pebble.h>
#include <time.h>
	
#define SERVER_URL "http://kije.cetus.uberspace.de/serv/NextTrain/NextTrainServer/"
	
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
	
#define foreach(x) for(uint16_t i = 0; i < COUNT_OF(x); i++)

	
#define changeEndian(x) (((x & 0x000000ff) << 24u) | ((x & 0x0000ff00) << 8u) | ((x & 0x00ff0000) >> 8u) | ((x & 0xff000000) >> 24u))
	

#endif