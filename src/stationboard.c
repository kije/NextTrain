#include "stationboard.h"
#include "keys.h"


Layer *stationboardLayer;
Layer *departureLayersContainer;
TextLayer *loadingLayer;
InverterLayer *inverterLayer;
DepartureLayer departureLayers[4];

static struct tm *t;
static time_t now;

uint8_t is_data_loaded = 0;


static TextLayer* text_layer_create_for_departure_layer(GRect frame, GTextAlignment text_alignment) {
	TextLayer* layer = text_layer_create(frame);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SOURCESANSPRO_REGULAR_10)));
	//text_layer_set_overflow_mode(layer, GTextOverflowModeTrailingEllipsis);
	text_layer_set_text_alignment(layer, text_alignment);
	text_layer_set_text_color(layer, GColorWhite);
	
	
	return layer;
}

static void update_departure_layer_data(DepartureLayer *layer) {
	Departure data = (*layer).departure;
	
	text_layer_set_text((*layer).from, data.from);
	text_layer_set_text((*layer).to, data.to);
	text_layer_set_text((*layer).category, data.category);
	
	static char time_buffer[] = "00:00";	
	strftime(time_buffer, sizeof(time_buffer), "%R",  data.time);	
	text_layer_set_text((*layer).departureTime, time_buffer);
	
	static char delay_buffer[4];
	snprintf(delay_buffer, sizeof(delay_buffer), "+%d", data.delay);
	
	text_layer_set_text((*layer).delay, delay_buffer);
	
	static char platform_buffer[10];
	snprintf(platform_buffer, sizeof(platform_buffer), "Gl. %s", data.platform);
	text_layer_set_text((*layer).platform, platform_buffer);

}

static void departure_layer_root_update_proc(struct Layer *layer, GContext *ctx) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Update departure_layer");
	if (is_data_loaded == 1) {
		GRect bounds = layer_get_bounds(layer);
		graphics_context_set_stroke_color(ctx,GColorWhite);
		graphics_draw_line(ctx, GPoint(0,bounds.size.h-1), GPoint(bounds.size.w-1,bounds.size.h-1));
	}
	
	/*uint16_t* index = layer_get_data(layer);
	update_departure_layer_data(&departureLayers[*index]);*/
}


static DepartureLayer departure_layer_create(GRect frame, uint16_t index) {
	now = time(NULL);
  	t = localtime(&now);
	DepartureLayer layer = (DepartureLayer) {
		.rootLayer = layer_create(frame),
		.from = text_layer_create_for_departure_layer(GRect(1,1,80,11), GTextAlignmentLeft),
		.to = text_layer_create_for_departure_layer(GRect(1,12,80,11), GTextAlignmentLeft),
		.departureTime = text_layer_create_for_departure_layer(GRect(82,1,24,11), GTextAlignmentLeft),
		.arivalTime = text_layer_create_for_departure_layer(GRect(82,12,24,11), GTextAlignmentLeft),
		.delay = text_layer_create_for_departure_layer(GRect(107,1,18,11), GTextAlignmentCenter),
		.category = text_layer_create_for_departure_layer(GRect(126,1,18,11), GTextAlignmentCenter),
		.platform = text_layer_create_for_departure_layer(GRect(107,12,37,11), GTextAlignmentCenter),
		.departure = (Departure){
			.from = "",
			.to = "",
			.category = "",
			.time = t,
			.delay = 0,
			.platform = ""
		}
	};
	
	
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.from));
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.to));
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.departureTime));
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.arivalTime));
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.delay));
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.category));
	layer_add_child(layer.rootLayer, text_layer_get_layer(layer.platform));
	

	layer_set_update_proc(layer.rootLayer, departure_layer_root_update_proc);
	
	return layer;
}

static void departure_layer_destroy(DepartureLayer layer) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Destroy departure_layer");
	text_layer_destroy(layer.to);
	text_layer_destroy(layer.from);
	text_layer_destroy(layer.departureTime);
	text_layer_destroy(layer.arivalTime);
	text_layer_destroy(layer.delay);
	text_layer_destroy(layer.category);
	text_layer_destroy(layer.platform);
	
	
	free(layer.departure.from);
	free(layer.departure.to);
	free(layer.departure.category);
	free(layer.departure.time);
	free(layer.departure.platform);
	
	layer_destroy(layer.rootLayer);
}

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
	
	/*uint8_t buffer[512];
	uint32_t size = sizeof(buffer);
	Tuple *tuple = dict_read_begin_from_buffer(received, buffer, size);
	
	uint8_t num_departures = 0;
	while (tuple != NULL && num_departures <= COUNT_OF(departures)) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "KEY: %ld", (int32_t)tuple->key); // unexpected keys: 1 -> 524622, 10 -> 536977380, etc... 
		if (tuple->key < PEBBLE_RESULT_END_KEY){
			if (tuple->key > PEBBLE_RESULT_START_KEY) {
				
				*
				struct Departure {
					char *from;
					char *to;
					char *category;
					struct tm *time;
					uint16_t delay;
					uint16_t platform;
				};
				/
				
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
	}*/
	now = time(NULL);
  	t = localtime(&now);
	
	is_data_loaded = 1;
	layer_set_hidden(text_layer_get_layer(loadingLayer), true);
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Filling departures");
	foreach(departureLayers) {
		departureLayers[i].departure = (Departure){
			.from = "Eptingen, Gemeindeplatz",
			.to = "Sissach, Bahnhof",
			.category = "BUS",
			.time = t,
			.delay = i,
			.platform = "1"
		};
		update_departure_layer_data(&departureLayers[i]);
	}
	
	//layer_mark_dirty(departureLayersContainer);

	
}


static void in_dropped_handler(AppMessageResult reason, void *context) {
	// incoming message dropped
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "%s Reason: %d", __FUNCTION__, reason);
}



void init_stationboard(Window *window) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init Stationboard");
	
	stationboardLayer = layer_create(GRect(0,55,144,113));
	
	GRect departuresBounds = GRect(0,13,144,100);
	departureLayersContainer = layer_create(departuresBounds);
	
	
	foreach(departureLayers) {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "%d", i);
		int16_t cell_height = departuresBounds.size.h/COUNT_OF(departureLayers);
		
		departureLayers[i] = departure_layer_create(GRect(0,cell_height*i,departuresBounds.size.w,cell_height), i);
		layer_add_child(departureLayersContainer, departureLayers[i].rootLayer);
	}
	
	layer_add_child(stationboardLayer, departureLayersContainer);
	
	loadingLayer = text_layer_create(departuresBounds);
	text_layer_set_background_color(loadingLayer, GColorClear);
	text_layer_set_font(loadingLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANCHEZ_REGULAR_13)));
	text_layer_set_text(loadingLayer, "Laden ...");
	text_layer_set_text_alignment(loadingLayer, GTextAlignmentCenter);
	text_layer_set_text_color(loadingLayer, GColorWhite);
	layer_add_child(stationboardLayer, text_layer_get_layer(loadingLayer));
		
	inverterLayer = inverter_layer_create(departuresBounds);
	//layer_add_child(stationboardLayer, (Layer *)inverterLayer);
	
	layer_add_child(window_get_root_layer(window), stationboardLayer);
	
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
	layer_destroy(departureLayersContainer);
	text_layer_destroy(loadingLayer);
	inverter_layer_destroy(inverterLayer);
	
	app_message_deregister_callbacks();
	free(t);
	foreach(departureLayers) {
		departure_layer_destroy(departureLayers[i]);
	}
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Memory usage of data: %d", sizeof(departureLayers));
}