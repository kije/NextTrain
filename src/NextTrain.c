#include "NextTrain.h"
#include "stationboard.h"
#include "clock.h"

	
// Global vars
Window *window;


static void init(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init");
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_stack_push(window, true /* Animated */);
	
	
	init_clock(window);
	init_stationboard(window);
}
	
void deinit(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deinit");
	deinit_clock();
	deinit_stationboard();
	window_destroy(window);
}


int main(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Start");
	init();
	app_event_loop();
	deinit();
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Done");
	return 0;
}