#include "stationboard.h"
#include "keys.h"


Layer *stationboardLayer;
Layer *departureLayersContainer;
TextLayer *loadingLayer;
InverterLayer *inverterLayer;
DepartureLayer departureLayers[4];


uint8_t is_data_loaded = 0;

uint16_t data_loads = 0;


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
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Update layer data");
	Departure data = layer->departure;
	
	text_layer_set_text(layer->from, data.from);
	text_layer_set_text(layer->to, data.to);
	text_layer_set_text(layer->category, data.category);
	
	static char departureTime_buffer[] = "00:00";	
	strftime(departureTime_buffer, sizeof(departureTime_buffer), "%R",  data.departureTime);	
	text_layer_set_text(layer->departureTime, departureTime_buffer);
	
	static char arivalTime_buffer[] = "00:00";	
	strftime(arivalTime_buffer, sizeof(arivalTime_buffer), "%R",  data.arivalTime);	
	text_layer_set_text((*layer).arivalTime, arivalTime_buffer); 
	
	static char delay_buffer[4];
	if (data.delay != 0) {
		snprintf(delay_buffer, sizeof(delay_buffer), "+%d", data.delay);
	} 
	
	text_layer_set_text(layer->delay, delay_buffer);
	
	static char platform_buffer[10];
	snprintf(platform_buffer, sizeof(platform_buffer), (strcmp(data.platform,"") == 0? "%s" : "Gl. %s"), data.platform);
	text_layer_set_text(layer->platform, platform_buffer);

}


static void departure_layer_root_update_proc(struct Layer *layer, GContext *ctx) {
	if (is_data_loaded == 1) {
		GRect bounds = layer_get_bounds(layer);
		graphics_context_set_stroke_color(ctx,GColorWhite);
		graphics_draw_line(ctx, GPoint(0,bounds.size.h-1), GPoint(bounds.size.w-1,bounds.size.h-1));
	}
	
	/*uint16_t* index = layer_get_data(layer);
	update_departure_layer_data(&departureLayers[*index]);*/
}


static DepartureLayer departure_layer_create(GRect frame, uint16_t index) {
	
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
			.from = (char *)malloc(35*sizeof(char)),
			.to = (char *)malloc(35*sizeof(char)),
			.category = (char *)malloc(3*sizeof(char)),
			.departureTime = (struct tm*)malloc(sizeof(struct tm)),
			.arivalTime = (struct tm*)malloc(sizeof(struct tm)),
			.delay = 0,
			.platform = (char *)malloc(9*sizeof(char))
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

static void departure_layer_destroy(DepartureLayer *layer) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Destroy departure_layer");
	text_layer_destroy(layer->to);
	text_layer_destroy(layer->from);
	text_layer_destroy(layer->departureTime);
	text_layer_destroy(layer->arivalTime);
	text_layer_destroy(layer->delay);
	text_layer_destroy(layer->category);
	text_layer_destroy(layer->platform);
	
	
	free(layer->departure.from);
	free(layer->departure.to);
	free(layer->departure.category);
	free(layer->departure.departureTime);
	free(layer->departure.arivalTime);
	free(layer->departure.platform);
	
	layer_destroy(layer->rootLayer);
	
	free(layer);
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
	uint8_t index = data_loads%COUNT_OF(departureLayers);
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Receive data for slot %d", index);

	Tuple *tuple = dict_read_first(received);
	time_t time;
	do {
		app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "KEY: %li, Adress: %p", tuple->key, &tuple);
		switch (tuple->key) {
			case PEBBLE_JS_STATION_NAME:
				memmove(departureLayers[index].departure.from, tuple->value->cstring, COUNT_OF(departureLayers[index].departure.from) * sizeof(departureLayers[index].departure.from));
				//*departureLayers[index].departure.from = *tuple->value->cstring;
			break;
			
			case PEBBLE_JS_DELAY:
				departureLayers[index].departure.delay = tuple->value->uint16;
			break;
			
			case PEBBLE_JS_PLATFORM:
				memmove(departureLayers[index].departure.platform, tuple->value->cstring, COUNT_OF(departureLayers[index].departure.platform) * sizeof(departureLayers[index].departure.platform));
			break;
			
			case PEBBLE_JS_TO:
				memmove(departureLayers[index].departure.to, tuple->value->cstring, COUNT_OF(departureLayers[index].departure.to) * sizeof(departureLayers[index].departure.to));
			break;
			
			case PEBBLE_JS_CATEGORY:
				memmove(departureLayers[index].departure.category, tuple->value->cstring, COUNT_OF(departureLayers[index].departure.category) * sizeof(departureLayers[index].departure.category));
			break;
			
			case PEBBLE_JS_DEPARTURE_TIME:
				time = tuple->value->int32;

				*departureLayers[index].departure.departureTime = *localtime(&time);
				app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Address: %p Size: %d", departureLayers[index].departure.departureTime, sizeof(departureLayers[index].departure.departureTime));
				
				app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "time: %d:%d", departureLayers[index].departure.departureTime->tm_hour, departureLayers[index].departure.departureTime->tm_min);
			break;
			
			case PEBBLE_JS_ARIVAL_TIME:
				time = tuple->value->int32;
				*departureLayers[index].departure.arivalTime = *localtime(&time);
			
			break;
		} 
	} while ((tuple = dict_read_next(received)) != NULL);
	
	free(tuple);

	is_data_loaded = 1;
	layer_set_hidden(text_layer_get_layer(loadingLayer), true);
	data_loads++;
	update_departure_layer_data(&(departureLayers[index]));
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
	text_layer_set_text(loadingLayer, "Lade... \n\n© by kije\n\nDaten: transport.opendata.ch");
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
	
	
	const uint32_t inbound_size = 512;
	const uint32_t outbound_size = 512;
	app_message_open(inbound_size, outbound_size);
}

void deinit_stationboard(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deinit Stationboard");
	layer_destroy(stationboardLayer);
	layer_destroy(departureLayersContainer);
	text_layer_destroy(loadingLayer);
	inverter_layer_destroy(inverterLayer);
	
	app_message_deregister_callbacks();
	foreach(departureLayers) {
		departure_layer_destroy(&departureLayers[i]);
	}
	
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Memory usage of data: %d", sizeof(departureLayers));
}