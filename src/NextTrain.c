#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"

	
Window window;

#define MY_UUID { 0x5B, 0x08, 0xBC, 0x25, 0xA5, 0x8E, 0x4E, 0x1C, 0xB4, 0xC5, 0x2A, 0x22, 0x34, 0xAB, 0xDF, 0x85 }
PBL_APP_INFO(MY_UUID,
             "NextTrain", "kije dev",
             0, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);



void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}