#include "clock.h"
#include "words.h"
	
TextLayer *timeLayer, *dateLayer;

static struct tm *t;
static time_t now;
	
static void time_update() {
	now = time(NULL);
  	t = localtime(&now);
	
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

void init_clock(Window *window) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init clock");
	timeLayer = text_layer_create(GRect(0,1,144,29));
	text_layer_set_text_alignment(timeLayer, GTextAlignmentCenter);
    text_layer_set_text(timeLayer, "12:34:56");
    text_layer_set_font(timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_MONO_BOLD_29)));
	text_layer_set_background_color(timeLayer, GColorClear);
    text_layer_set_text_color(timeLayer, GColorWhite);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
	
	dateLayer = text_layer_create(GRect(0,35,144,17));
	text_layer_set_text_alignment(dateLayer, GTextAlignmentCenter);
    text_layer_set_text(dateLayer, "Samstag, 28. Nov");
    text_layer_set_font(dateLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANCHEZ_REGULAR_13)));
	text_layer_set_background_color(dateLayer, GColorClear);
    text_layer_set_text_color(dateLayer, GColorWhite);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(dateLayer));
	
	time_update();
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

void deinit_clock(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deinit clock");
	tick_timer_service_unsubscribe();
	text_layer_destroy(timeLayer);
	text_layer_destroy(dateLayer);
	
	
	free(t);
} 