#include "stationboard.h"
#include "keys.h"

Layer *stationboardLayer;
InverterLayer *inverterLayer;
Departure departures[3];


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
	
	uint8_t buffer[512];
	uint32_t size = sizeof(buffer);
	Tuple *tuple = dict_read_begin_from_buffer(received, buffer, size);
	
	uint8_t num_departures = 0;
	while (tuple != NULL && num_departures <= COUNT_OF(departures)) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "KEY: %10lu", tuple->key); // unexpected keys: 1 -> 524622, 10 -> 536977380, etc... 
		if (tuple->key < PEBBLE_RESULT_END_KEY){
			if (tuple->key > PEBBLE_RESULT_START_KEY) {
				
				/*
				struct Departure {
					char *from;
					char *to;
					char *category;
					struct tm *time;
					uint16_t delay;
					uint16_t platform;
				};
				*/
				
				switch ((tuple->key-PEBBLE_RESULT_START_KEY) % PEBBLE_RESERVED_FIELDS_PER_RESULT) {
					case PEBBLE_RESULT_NTH_SATION_NAME:
						departures[num_departures].from = tuple->value->cstring;
						break;
					
					case PEBBLE_RESULT_NTH_DELAY:
						departures[num_departures].delay = tuple->value->uint16;
						break;
					
					case PEBBLE_RESULT_NTH_PLATFORM:
						departures[num_departures].platform = tuple->value->uint16;
						break;
					
					case PEBBLE_RESULT_NTH_TO:
						departures[num_departures].to = tuple->value->cstring;
						break;
					
					case PEBBLE_RESULT_NTH_CATEGORY:
						departures[num_departures].category = tuple->value->cstring;
						break;
					
					case PEBBLE_RESULT_NTH_DEPARTURE_HOUR:
						departures[num_departures].time.tm_hour = tuple->value->int16;
						break;
					
					case PEBBLE_RESULT_NTH_DEPARTURE_MIN:
						departures[num_departures].time.tm_min = tuple->value->int16;
						break;
					
					case PEBBLE_RESULT_NTH_DEPARTURE_DST:
						departures[num_departures].time.tm_isdst = tuple->value->int16;
						break;
					
					case PEBBLE_RESULT_NTH_DEPARTURE_DAY:
						departures[num_departures].time.tm_mday = tuple->value->int16;
						break;
					
					case PEBBLE_RESULT_NTH_DEPARTURE_MONTH:
						departures[num_departures].time.tm_mon = tuple->value->int16;
						break;
				}
				num_departures = (uint8_t)((tuple->key-PEBBLE_RESULT_START_KEY) / PEBBLE_RESERVED_FIELDS_PER_RESULT);
				app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "%s num_dep: %d", __FUNCTION__, num_departures);
			}
		} else {
			//break;
		}
		//tuple->key
		//tuple->value->data, tuple->length
	  	tuple = dict_read_next(received);
	}
}


static void in_dropped_handler(AppMessageResult reason, void *context) {
	// incoming message dropped
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "%s Reason: %d", __FUNCTION__, reason);
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