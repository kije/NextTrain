#include "stationboard.h"
#include "keys.h"


Layer *stationboardLayer;
Layer *departureLayersContainer;
TextLayer *loadingLayer;

Layer *table_header_layer;
TextLayer *status_description_layer, *time_description_layer, *dtp_description_layer;

InverterLayer *inverterLayer;
DepartureLayer departureLayers[4];


uint8_t is_data_loaded = 0;

uint16_t data_loads = 0;


static TextLayer* text_layer_create_for_departure_layer(GRect frame, GTextAlignment text_alignment) {
	TextLayer* layer = text_layer_create(frame);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_9)));
	//text_layer_set_overflow_mode(layer, GTextOverflowModeTrailingEllipsis);
	text_layer_set_text_alignment(layer, text_alignment);
	text_layer_set_text_color(layer, GColorWhite);
	
	
	return layer;
}


void update_departure_layer_data(DepartureLayer *layer) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Update layer data");
	Departure data = layer->departure;
	
	text_layer_set_text(layer->from, data.from);
	text_layer_set_text(layer->to, data.to);
	text_layer_set_text(layer->category, data.category);
	
	
	strftime(data.departure_time_buffer, COUNT_OF(data.departure_time_buffer) * sizeof(data.departure_time_buffer), "%R",  data.departureTime);	
	text_layer_set_text(layer->departureTime, data.departure_time_buffer);
	
	strftime(data.arival_time_buffer, COUNT_OF(data.arival_time_buffer) * sizeof(data.arival_time_buffer), "%R",  data.arivalTime);
	text_layer_set_text(layer->arivalTime, data.arival_time_buffer); 
	
	if (data.delay != 0) {
		snprintf(data.delay_buffer, COUNT_OF(data.delay_buffer) * sizeof(data.delay_buffer), "+%d", data.delay);
	} 
	
	text_layer_set_text(layer->delay, data.delay_buffer);
	
	snprintf(data.platform_buffer, COUNT_OF(data.platform_buffer) * sizeof(data.platform_buffer), (strcmp(data.platform,"") == 0? "%s" : "Gl. %s"), data.platform);
	text_layer_set_text(layer->platform, data.platform_buffer);

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


DepartureLayer departure_layer_create(GRect frame, uint16_t index) {
	Departure dep = (Departure){
		.from = (char *)malloc(40*sizeof(char)),
		.to = (char *)malloc(40*sizeof(char)),
		.category = (char *)malloc(3*sizeof(char)),
		.departureTime = (struct tm*)malloc(sizeof(struct tm)),
		.arivalTime = (struct tm*)malloc(sizeof(struct tm)),
		.delay = 0,
		.platform = (char *)malloc(9*sizeof(char)),
		
		.arival_time_buffer = (char *)malloc(5*sizeof(char)),
		.departure_time_buffer = (char *)malloc(5*sizeof(char)),
		.delay_buffer = (char *)malloc(5*sizeof(char)),
		.platform_buffer = (char *)malloc(11*sizeof(char))
	};
	DepartureLayer layer = (DepartureLayer) {
		.rootLayer = layer_create(frame),
		.from = text_layer_create_for_departure_layer(GRect(1,0,80,10), GTextAlignmentLeft),
		.to = text_layer_create_for_departure_layer(GRect(1,10,80,10), GTextAlignmentLeft),
		.departureTime = text_layer_create_for_departure_layer(GRect(82,0,25,10), GTextAlignmentLeft),
		.arivalTime = text_layer_create_for_departure_layer(GRect(82,10,25,10), GTextAlignmentLeft),
		.delay = text_layer_create_for_departure_layer(GRect(107,0,17,10), GTextAlignmentCenter),
		.category = text_layer_create_for_departure_layer(GRect(125,0,18,10), GTextAlignmentRight),
		.platform = text_layer_create_for_departure_layer(GRect(107,10,36,10), GTextAlignmentCenter),
		.departure = dep
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
	app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "\tDestroy Layers");
	text_layer_destroy(layer->to);
	text_layer_destroy(layer->from);
	text_layer_destroy(layer->departureTime);
	text_layer_destroy(layer->arivalTime);
	text_layer_destroy(layer->delay);
	text_layer_destroy(layer->category);
	text_layer_destroy(layer->platform);
	
	app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "\tDestroy Data");
	free(layer->departure.from);
	free(layer->departure.to);
	free(layer->departure.category);
	free(layer->departure.departureTime);
	free(layer->departure.arivalTime);
	free(layer->departure.platform);
	
	//Buffers
	/*app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "\tDestroy Buffers");
	free(layer->departure.arival_time_buffer);
	free(layer->departure.departure_time_buffer);
	free(layer->departure.delay_buffer);
	free(layer->departure.platform_buffer);*/
	
	app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "\tDestroy Layer");
	layer_destroy(layer->rootLayer);

}

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , __FUNCTION__);
}


static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	// outgoing message failed
	app_log(APP_LOG_LEVEL_WARNING, __FILE__ , __LINE__ , __FUNCTION__);
}


void in_received_handler(DictionaryIterator *received, void *context) {
    uint8_t index = data_loads%COUNT_OF(departureLayers);
    app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Receive data for slot %d", index);
	

    Tuple *tuple;
    time_t time;
    while ((tuple = dict_read_next(received)) != NULL) {
        app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "KEY: %li\t Array-El mem pos: %p", tuple->key, &departureLayers[index]);
        switch (tuple->key) {
            case PEBBLE_JS_STATION_NAME:
                memmove(departureLayers[index].departure.from, tuple->value->cstring, COUNT_OF(departureLayers[index].departure.from) * sizeof(departureLayers[index].departure.from));
				app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "st name: %s \t st in t: %s", departureLayers[index].departure.from, tuple->value->cstring);
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
				memmove(departureLayers[index].departure.departureTime, localtime(&time), sizeof(struct tm));
				
            break;
            
            case PEBBLE_JS_ARIVAL_TIME:
                time = tuple->value->int32;
                *departureLayers[index].departure.arivalTime = *localtime(&time);
				
            
            break;
        } 
    }
    
    free(tuple);

    is_data_loaded = 1;
    layer_set_hidden(text_layer_get_layer(loadingLayer), true);
    data_loads++;
    update_departure_layer_data(&departureLayers[index]);
}


static void in_dropped_handler(AppMessageResult reason, void *context) {
	// incoming message dropped
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "%s Reason: %d", __FUNCTION__, reason);
}



void init_stationboard(Window *window) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Init Stationboard");
	
	stationboardLayer = layer_create(GRect(0,55,144,113));
	
	GRect departuresBounds = GRect(0,20,144,93);
	
	departureLayersContainer = layer_create(departuresBounds);
	
	/*
		GRect(1,1,80,11) from to
		GRect(82,1,25,11) time
		GRect(107,1,17,11) delay/type/platform
	*/
	
	table_header_layer = layer_create(GRect(0,0,144,23));
	status_description_layer = text_layer_create_for_departure_layer(GRect(1,0,80,20), GTextAlignmentLeft);
	time_description_layer = text_layer_create_for_departure_layer(GRect(82,0,25,20), GTextAlignmentLeft);
	dtp_description_layer = text_layer_create_for_departure_layer(GRect(107,0,36,20), GTextAlignmentCenter);
	
	text_layer_set_text(status_description_layer, "Von\nNach");
	text_layer_set_text(time_description_layer, "\nZeit");
	text_layer_set_text(dtp_description_layer, "V      T\nGleis");
	
	layer_add_child(table_header_layer, text_layer_get_layer(status_description_layer));
	layer_add_child(table_header_layer, text_layer_get_layer(time_description_layer));
	layer_add_child(table_header_layer, text_layer_get_layer(dtp_description_layer));
	
	
	layer_add_child(stationboardLayer, table_header_layer);
	
	
	int16_t cell_height = departuresBounds.size.h/COUNT_OF(departureLayers);
	foreach(departureLayers) {
		
		departureLayers[i] = departure_layer_create(GRect(0,cell_height*i,departuresBounds.size.w,cell_height), i);
		layer_add_child(departureLayersContainer, departureLayers[i].rootLayer);
	}
	
	layer_add_child(stationboardLayer, departureLayersContainer);
	
	loadingLayer = text_layer_create(departuresBounds);
	text_layer_set_background_color(loadingLayer, GColorClear);
	text_layer_set_font(loadingLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANCHEZ_REGULAR_13)));
	text_layer_set_text(loadingLayer, "Lade …");
	text_layer_set_text_alignment(loadingLayer, GTextAlignmentCenter);
	text_layer_set_text_color(loadingLayer, GColorWhite);
	layer_add_child(stationboardLayer, text_layer_get_layer(loadingLayer));
		
	inverterLayer = inverter_layer_create(departuresBounds);
	layer_add_child(stationboardLayer, (Layer *)inverterLayer);
	
	layer_add_child(window_get_root_layer(window), stationboardLayer);
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
	
	
	const uint32_t inbound_size = 256;
	const uint32_t outbound_size = 256;
	app_message_open(inbound_size, outbound_size);
}

void deinit_stationboard(void) {
	app_log(APP_LOG_LEVEL_DEBUG, __FILE__ , __LINE__ , "Deinit Stationboard");
	app_message_deregister_callbacks();
	
	text_layer_destroy(status_description_layer);
	text_layer_destroy(time_description_layer);
	text_layer_destroy(dtp_description_layer);
	layer_destroy(table_header_layer);
	
	layer_destroy(stationboardLayer);
	layer_destroy(departureLayersContainer);
	text_layer_destroy(loadingLayer);
	inverter_layer_destroy(inverterLayer);
	
	
	foreach(departureLayers) {
		departure_layer_destroy(&departureLayers[i]);
	}
	
	app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE__ , __LINE__ , "Memory usage of data: %d", sizeof(departureLayers));
}