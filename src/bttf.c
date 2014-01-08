#include <pebble.h>

#define kDestinationTime1 "JAN 12 14 10:30"
#define kLastTime1        "JAN 11 14 08:33"

#define kDestinationTime2 "JAN 12 14 10 30"
#define kLastTime2        "JAN 11 14 08 33"

static Window *window;
static TextLayer *top_labels_layer;
static TextLayer *destination_label_layer;
static TextLayer *present_time_label_layer;
static TextLayer *last_time_label_layer;

static TextLayer *dest_time_layer;
static TextLayer *present_time_layer;
static TextLayer *last_time_layer;

static Layer *line1_layer;
static Layer *line2_layer;

static Layer *present_ampm_layer;
static Layer *destination_ampm_layer;
static Layer *last_ampm_layer;

static bool show_colons;


static void line_layer_draw(Layer *l, GContext *ctx) {
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorWhite);
  GRect bounds = layer_get_bounds(l);

	graphics_draw_line(ctx, (GPoint) { .x = 0, .y = 0 }, (GPoint) { .x = bounds.size.w, .y = 0 } );
}


static void tick(struct tm *tick_time, TimeUnits units_changed) {
	static char time_text[] = "OCT 26 85 01:22";
	static char dest_time_text[] = kDestinationTime1;
	static char last_time_text[] = kLastTime1;

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "tick");
	
	show_colons = !show_colons;

	if (show_colons) { 
		// %b for month
		strftime(time_text, sizeof(time_text), "JAN %d 85 %I:%M", tick_time);
		strcpy(dest_time_text, kDestinationTime1);
		strcpy(last_time_text, kLastTime1);
	}
	else {
		strftime(time_text, sizeof(time_text), "JAN %d 85 %I %M", tick_time);
		strcpy(dest_time_text, kDestinationTime2);
		strcpy(last_time_text, kLastTime2);
	}

	text_layer_set_text(present_time_layer, time_text);
	text_layer_set_text(dest_time_layer, dest_time_text);
	text_layer_set_text(last_time_layer, last_time_text);

	layer_mark_dirty(present_ampm_layer);
}

static void always_am_layer_draw(Layer *l, GContext *ctx) {
	graphics_context_set_stroke_color(ctx, GColorWhite);

	graphics_draw_pixel(ctx, (GPoint) { .x = 0, .y = 1 });
	graphics_draw_pixel(ctx, (GPoint) { .x = 0, .y = 0 });
	graphics_draw_pixel(ctx, (GPoint) { .x = 1, .y = 1 });
	graphics_draw_pixel(ctx, (GPoint) { .x = 1, .y = 0 });
}

static void present_ampm_layer_draw(Layer *l, GContext *ctx) {
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);

	graphics_context_set_stroke_color(ctx, GColorWhite);

	if (current_time->tm_hour >= 12) {
		graphics_draw_pixel(ctx, (GPoint) { .x = 0, .y = 7 });
		graphics_draw_pixel(ctx, (GPoint) { .x = 0, .y = 6 });
		graphics_draw_pixel(ctx, (GPoint) { .x = 1, .y = 7 });
		graphics_draw_pixel(ctx, (GPoint) { .x = 1, .y = 6 });
	}
	else {
		graphics_draw_pixel(ctx, (GPoint) { .x = 0, .y = 1 });
		graphics_draw_pixel(ctx, (GPoint) { .x = 0, .y = 0 });
		graphics_draw_pixel(ctx, (GPoint) { .x = 1, .y = 1 });
		graphics_draw_pixel(ctx, (GPoint) { .x = 1, .y = 0 });
	}
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
	window_set_background_color(window, GColorBlack);

  GRect bounds = layer_get_bounds(window_layer);

	top_labels_layer = text_layer_create( (GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 20 } } );
	text_layer_set_text(top_labels_layer, "MONTH DAY YEAR HOUR MIN");
	text_layer_set_text_alignment(top_labels_layer, GTextAlignmentLeft);
	text_layer_set_font(top_labels_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_background_color(top_labels_layer, GColorBlack);
	text_layer_set_text_color(top_labels_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(top_labels_layer));
	

	destination_label_layer = text_layer_create( (GRect) { .origin = { 0, 50 }, .size = { bounds.size.w, 20 } } );
	text_layer_set_text(destination_label_layer, "Destination Time");
	text_layer_set_text_alignment(destination_label_layer, GTextAlignmentCenter);
	text_layer_set_background_color(destination_label_layer, GColorBlack);
	text_layer_set_text_color(destination_label_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(destination_label_layer));


	present_time_label_layer = text_layer_create( (GRect) { .origin = { 0, 102 }, .size = { bounds.size.w, 20 } } );
	text_layer_set_text(present_time_label_layer, "Present Time");
	text_layer_set_text_alignment(present_time_label_layer, GTextAlignmentCenter);
	text_layer_set_background_color(present_time_label_layer, GColorBlack);
	text_layer_set_text_color(present_time_label_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(present_time_label_layer));


	last_time_label_layer = text_layer_create( (GRect) { .origin = { 0, 151 }, .size = { bounds.size.w, 20 } } );
	text_layer_set_text(last_time_label_layer, "Last Time Departed");
	text_layer_set_text_alignment(last_time_label_layer, GTextAlignmentCenter);
	text_layer_set_background_color(last_time_label_layer, GColorBlack);
	text_layer_set_text_color(last_time_label_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(last_time_label_layer));


	dest_time_layer = text_layer_create( (GRect) { .origin = { 0, 22 }, .size = { bounds.size.w, 26 } } );
	text_layer_set_font(dest_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(dest_time_layer, kDestinationTime1);
	text_layer_set_text_alignment(dest_time_layer, GTextAlignmentCenter);
	text_layer_set_background_color(dest_time_layer, GColorBlack);
	text_layer_set_text_color(dest_time_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(dest_time_layer));


	present_time_layer = text_layer_create( (GRect) { .origin = { 0, 74 }, .size = { bounds.size.w, 26 } } );
	text_layer_set_font(present_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(present_time_layer, "");
	text_layer_set_text_alignment(present_time_layer, GTextAlignmentCenter);
	text_layer_set_background_color(present_time_layer, GColorBlack);
	text_layer_set_text_color(present_time_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(present_time_layer));


	last_time_layer = text_layer_create( (GRect) { .origin = { 0, 126 }, .size = { bounds.size.w, 26 } } );
	text_layer_set_font(last_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(last_time_layer, kLastTime1);
	text_layer_set_text_alignment(last_time_layer, GTextAlignmentCenter);
	text_layer_set_background_color(last_time_layer, GColorBlack);
	text_layer_set_text_color(last_time_layer, GColorWhite);

	layer_add_child(window_layer, text_layer_get_layer(last_time_layer));


	line2_layer = layer_create( (GRect) { .origin = { 0, 123 }, .size = { bounds.size.w, 2 } } );
	line1_layer = layer_create( (GRect) { .origin = { 0, 71 }, .size = { bounds.size.w, 2 } } );
	layer_set_update_proc(line1_layer, line_layer_draw);
	layer_set_update_proc(line2_layer, line_layer_draw);

	layer_add_child(window_layer, line1_layer);
	layer_add_child(window_layer, line2_layer);


	present_ampm_layer = layer_create( (GRect) { .origin = { 144 - 53, 85 }, .size = { 2, 20 } } );
	layer_set_update_proc(present_ampm_layer, present_ampm_layer_draw);
	layer_add_child(window_layer, present_ampm_layer);

	destination_ampm_layer = layer_create( (GRect) { .origin = { 144 - 53, 33 }, .size = { 2, 20 } } );
	layer_set_update_proc(destination_ampm_layer, always_am_layer_draw);
	layer_add_child(window_layer, destination_ampm_layer);

	last_ampm_layer = layer_create( (GRect) { .origin = { 144 - 53, 137 }, .size = { 2, 20 } } );
	layer_set_update_proc(last_ampm_layer, always_am_layer_draw);
	layer_add_child(window_layer, last_ampm_layer);


	show_colons = false;

	tick_timer_service_subscribe(SECOND_UNIT, tick);

	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	tick(current_time, SECOND_UNIT);
}


static void window_unload(Window *window) {
  //text_layer_destroy(text_layer);
	text_layer_destroy(top_labels_layer);
	text_layer_destroy(destination_label_layer);
	text_layer_destroy(present_time_label_layer);
	text_layer_destroy(last_time_label_layer);

	text_layer_destroy(dest_time_layer);
	text_layer_destroy(present_time_layer);
	text_layer_destroy(last_time_layer);

	layer_destroy(line1_layer);
	layer_destroy(line2_layer);
}


static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}


static void deinit(void) {
  window_destroy(window);
}


int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
