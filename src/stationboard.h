#ifndef STATIONBOARD_H_INCLUDED
#define STATIONBOARD_H_INCLUDED

#include "header.h"

	
void init_stationboard(Window *window);
void deinit_stationboard(void);

typedef struct Departure Departure;
typedef struct DepartureLayer DepartureLayer;

struct Departure {
	char *from;
	char *to;
	char *category;
	struct tm *departureTime;
	struct tm *arivalTime;
	uint16_t delay;
	char *platform;
};

struct DepartureLayer {
	Layer* rootLayer;
	TextLayer* to;
	TextLayer* from;
	TextLayer* departureTime;
	TextLayer* arivalTime;
	TextLayer* delay;
	TextLayer* category;
	TextLayer* platform;
	Departure departure;
};
	
#endif