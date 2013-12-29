#include <pebble.h>
#include <time.h>
#include "header.h"

	
// Global vars
Window *window;
TextLayer *timeLayer, *dateLayer;


static void time_update() {
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);
	
	static char time_buffer[] = "00:00:00";	
	strftime(time_buffer, sizeof(time_buffer), "%T", t);	
	text_layer_set_text(timeLayer, time_buffer);
	
	char day[15] = "", month[7] = "";
	
	strcat(day, DAYS[t->tm_wday]);
	strcat(month, MONTHS[t->tm_mon]);
	
	static char date_buffer[30] = "";	
	snprintf(date_buffer, sizeof(date_buffer), "%s, %d. %s", day, t->tm_mday, month);
	
	text_layer_set_text(dateLayer, date_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	time_update();
}


static void window_load(Window *window) {
	timeLayer = text_layer_create(GRect(0,5,144,29));
	text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);
    text_layer_set_text(timeLayer, "12:34:56");
    text_layer_set_font(timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_NOVAMONO_28)));
	text_layer_set_background_color(timeLayer, GColorClear);
    text_layer_set_text_color(timeLayer, GColorWhite);
    layer_add_child(window_get_root_layer(window), (Layer *)timeLayer);
	
	dateLayer = text_layer_create(GRect(0,38,144,17));
	text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
    text_layer_set_text(dateLayer, "Samstag, 28. Nov");
    text_layer_set_font(dateLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANCHEZ_REGULAR_13)));
	text_layer_set_background_color(dateLayer, GColorClear);
    text_layer_set_text_color(dateLayer, GColorWhite);
    layer_add_child(window_get_root_layer(window), (Layer *)dateLayer);
	
	time_update();
}

static void window_unload(Window *window) {
	text_layer_destroy(timeLayer);
	text_layer_destroy(dateLayer);
}

static void init(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init");
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(window, true /* Animated */);
	
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}
	
void deinit(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deinit");
	tick_timer_service_unsubscribe();
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