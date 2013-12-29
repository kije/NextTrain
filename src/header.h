#pragma once
	
#define SERVER_URL "http://kije.cetus.uberspace.de/serv/NextTrain/NextTrainServer/"
	
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
	

const char* const DAYS[] = {
	"Sonntag",
	"Montag", 
	"Dienstag", 
	"Mittwoch", 
	"Donnerstag", 
	"Freitag", 
	"Samstag"
};

const char* const MONTHS[] = {
	"Jan",
	"Feb",
	"März",
	"April",
	"Mai",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Okt",
	"Nov",
	"Dez"
};