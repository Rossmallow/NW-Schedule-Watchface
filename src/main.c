#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_period_layer, *s_ends_layer, *s_left_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

char getPeriod();

static void update_time(int h, int m, int d) {

  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

	// Copy date into buffer from tm structure
	static char date_buffer[32];
	strftime(date_buffer, sizeof(date_buffer), "%a, %b %d, %Y", tick_time);
	
	// Create period buffer
	static char period_buffer[32];
	char periodState[] = "Period: -";
	if ((d != 0 && d <= 5)) {
		periodState[8] = getPeriod(h, m);
	}
	strftime(period_buffer, sizeof(period_buffer), periodState, tick_time);
	
	// Create ends buffer
	static char ends_buffer[32];
	char numbers[] = "0123456789";
	int hours = 0;
	int minutes = 0;
	
	if (getPeriod(h, m) == 'A'){
		hours = 0; 
		minutes = 0;
	}
	else if (getPeriod(h, m) == '9'){
		hours = 15;
		minutes = 23;
	}
	else if (getPeriod(h, m) == '8'){
		hours = 14; 
		minutes = 36;
	}
	else if (getPeriod(h, m) == '7'){
		hours = 13; 
		minutes = 49;
	}
	else if (getPeriod(h, m) == '6'){
		hours = 13; 
		minutes = 2;
	}
	else if (getPeriod(h, m) == '5'){
		hours = 12; 
		minutes = 15;
	}
	else if (getPeriod(h, m) == '4'){
		hours = 11; 
		minutes = 28;
	}
	else if (getPeriod(h, m) == 'H'){
		hours = 10; 
		minutes = 41;
	}
	else if (getPeriod(h, m) == '3'){
		hours = 10; 
		minutes = 26;
	}
	else if (getPeriod(h, m) == '2'){
		hours = 9; 
		minutes = 39;
	}
	else if (getPeriod(h, m) == '1' || getPeriod(h, m) == 'E'){
		hours = 8; 
		minutes = 52;
	}
	else if (getPeriod(h, m) == 'B'){
		hours = 7;
		minutes = 5;
	}
	char endsState[] = "Ends: --:--";
	int hours_12 = hours;
	if (hours > 12 && clock_is_24h_style() == false) {
		hours_12 -= 12;
	}
	if(getPeriod(h, m) != 'A' && (d != 0 && d <= 5)) {
		endsState[6] = numbers[hours_12 / 10];
		endsState[7] = numbers[hours_12 % 10];
		endsState[9] = numbers[minutes / 10];
		endsState[10] = numbers[minutes % 10];
	}
	strftime(ends_buffer, sizeof(ends_buffer), endsState , tick_time);
	
	// Create left buffer
	static char left_buffer[32];
	int endTime = (hours * 60) + minutes;
	int currentTime = (h * 60) + m;
	int difference = endTime - currentTime;
	
	char leftState[] = "Time Left: --:--";
	if (getPeriod(h, m) != 'A' && (d != 0 && d <= 5)) {
		leftState[11] = numbers[(difference / 60) / 10];
		leftState[12] = numbers[(difference / 60) % 10];
		leftState[14] = numbers[(difference % 60) / 10];
		leftState[15] = numbers[(difference % 60) % 10];
	}
	strftime(left_buffer, sizeof(left_buffer), leftState , tick_time);
	
	// Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	
	// Show the date
	text_layer_set_text(s_date_layer, date_buffer);
	
	// Show the period
	text_layer_set_text(s_period_layer, period_buffer);
	
	// Show the ends
	text_layer_set_text(s_ends_layer, ends_buffer);
	
	// Show the left
	text_layer_set_text(s_left_layer, left_buffer);
}

static void main_window_load(Window *window) {
	// Create GBitmap, then set to created BitmapLayer
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 0, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	// Create date TextLayer
	s_date_layer = text_layer_create(GRect(0, 115, 144, 50));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_text(s_date_layer, "Xxx, Xxx 00, 0000");
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	// Create period TextLayer
	s_period_layer = text_layer_create(GRect(0, 130, 144, 50));
	text_layer_set_background_color(s_period_layer, GColorClear);
	text_layer_set_text_color(s_period_layer, GColorWhite);
	text_layer_set_text(s_period_layer, "Period: 0");
	text_layer_set_font(s_period_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_period_layer, GTextAlignmentLeft);

	// Create ends TextLayer
	s_ends_layer = text_layer_create(GRect(0, 130, 144, 50));
	text_layer_set_background_color(s_ends_layer, GColorClear);
	text_layer_set_text_color(s_ends_layer, GColorWhite);
	text_layer_set_text(s_ends_layer, "Ends: 00:00");
	text_layer_set_font(s_ends_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_ends_layer, GTextAlignmentRight);
	
	//Create left TextLayer
	s_left_layer = text_layer_create(GRect(0, 145, 144, 50));
	text_layer_set_background_color(s_left_layer, GColorClear);
	text_layer_set_text_color(s_left_layer, GColorWhite);
	text_layer_set_text(s_left_layer, "Time Left: 00:00");
	text_layer_set_font(s_left_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_left_layer, GTextAlignmentCenter);
	
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_period_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ends_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_left_layer));
  
  // Make sure the time is displayed from the start
  update_time(0, 0, 0);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_period_layer);
	text_layer_destroy(s_ends_layer);
	text_layer_destroy(s_left_layer);
	
	// Destroy GBitmap
	gbitmap_destroy(s_background_bitmap);

	// Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	int hours = tick_time->tm_hour;
	int minutes = tick_time->tm_min;
	int day = tick_time->tm_wday;
	update_time(hours, minutes, day);
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

char getPeriod(int h, int m) {
	char period[] = " 123456789BEHA-";
	int t = (h * 60) + m;
	if (h == 0 && m == 0) {
		return period[14];}
	else if (t >= 923) {
		return period[13];}
	else if (t >= 876) {
		return period[9];}
	else if (t >= 829) {
		return period[8];}
	else if (t >= 782) {
		return period[7];}
	else if (t >= 735) {
		return period[6];}
	else if (t >= 688) {
		return period[5];}
	else if (t >= 641) {
		return period[4];}
	else if (t >= 626) {
		return period[12];}
	else if (t >= 579) {
		return period[3];}
	else if (t >= 532) {
		return period[2];}
	else if (t >= 490) {
		return period[1];}
	else if (t >= 425) {
		return period[11];}
	else if (t < 425) {
		return period[10];}
	return period[0];
}