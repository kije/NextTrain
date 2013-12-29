#include <pebble.h>
#include "header.h"


static void init(void) {
}
	
void deinit(void) {
}


int main(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Start");
	init();
	app_event_loop();
	deinit();
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Done");
	return 0;
}