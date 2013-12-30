#ifndef STATIONBOARD_H_INCLUDED
#define STATIONBOARD_H_INCLUDED

#include "header.h"

	
void init_stationboard(Window *window);
void deinit_stationboard(void);

typedef struct Departure Departure;

struct Departure {
	char *from;
	char *to;
	char *category;
	struct tm time;
	uint16_t delay;
	uint16_t platform;
};
	
#endif