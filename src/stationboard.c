#include "stationboard.h"

Layer *stationboardLayer;
InverterLayer *inverterLayer;


static void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , __FUNCTION__);
}


static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	// outgoing message failed
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , __FUNCTION__);
}


static void in_received_handler(DictionaryIterator *received, void *context) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , __FUNCTION__);
	// incoming message received
	Tuple *status_tuple = dict_find(received, 1);

	// Act on the found fields received
	if (status_tuple) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Status: %d", status_tuple->value->uint16 );
	}
}


static void in_dropped_handler(AppMessageResult reason, void *context) {
	// incoming message dropped
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "%s", (reason==APP_MSG_BUFFER_OVERFLOW ? "BUFFER_OVERFLOW": "nope") );
}

static void stationboard_update_proc(Layer *layer, GContext *ctx) {
	
}

void init_stationboard(Window *window) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init Stationboard");
	
	stationboardLayer = layer_create(GRect(0,55,144,113));
	layer_set_update_proc(stationboardLayer, stationboard_update_proc);
	layer_add_child(window_get_root_layer(window), stationboardLayer);
	
	inverterLayer = inverter_layer_create(GRect(0,13,144,100));
	layer_add_child(stationboardLayer, (Layer *)inverterLayer);
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	
	const uint32_t inbound_size = 1024;
	const uint32_t outbound_size = 1024;
	app_message_open(inbound_size, outbound_size);
}

void deinit_stationboard(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deinit Stationboard");
	layer_destroy(stationboardLayer);
}