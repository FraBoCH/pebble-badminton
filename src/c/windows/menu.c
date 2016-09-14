#include "menu.h"

static Window *s_main_window;

static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection main_menu_sections[2];
static SimpleMenuItem main_menu_items[1];
static SimpleMenuItem main_menu_item_options[2];

static Settings settings;

const char *setting_to_string(int n) {
  switch (n) {
    case 1: return "1";
    case 3: return "3";
    case 15: return "15";
    case 21: return "21";
    default: return "?";
  }
}

void cycle_set_num() {
  switch (settings.num_sets) {
    case 1: 
      settings.num_sets = 3;
      break;
    case 3: 
      settings.num_sets = 1;
      break;
  }
  main_menu_item_options[0].subtitle = setting_to_string(settings.num_sets);
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void cycle_points() {
  switch (settings.num_points) {
    case 15: 
      settings.num_points = 21;
      break;
    case 21: 
      settings.num_points = 15;
      break;
    default:
      settings.num_points = 99;
      break;
  }
  // compute max_points depending on num points according to badminton law
  settings.max_points = settings.num_points == 21 ? 30 : settings.num_points + 5;
  
  main_menu_item_options[1].subtitle = setting_to_string(settings.num_points);
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void start_match() {
  window_stack_pop(false);
  match_window_push(&settings, list_new());
  save_settings(&settings);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  settings = (Settings)
    { .num_sets = 3
    , .num_points = 21
    , .max_points = 30
    , .first_server = PLAYER
    };

  load_settings(&settings);

  main_menu_sections[0] = (SimpleMenuSection) {
    .num_items = 1,
    .items = main_menu_items
  };

  main_menu_items[0] = (SimpleMenuItem) {
    .title = "Start Match",
    .callback = start_match
  };

  main_menu_sections[1] = (SimpleMenuSection) {
    .title = "Match Settings",
    .items = main_menu_item_options,
    .num_items = 2
  };

  main_menu_item_options[0] = (SimpleMenuItem) {
    .title = "Sets",
    .subtitle = setting_to_string(settings.num_sets),
    .callback = cycle_set_num
  };
  
  main_menu_item_options[1] = (SimpleMenuItem) {
    .title = "Points",
    .subtitle = setting_to_string(settings.num_points),
    .callback = cycle_points
  };
  
  main_menu_layer = simple_menu_layer_create(bounds, window, main_menu_sections, 2, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(main_menu_layer));

#ifdef PBL_COLOR
  menu_layer_set_highlight_colors(simple_menu_layer_get_menu_layer(main_menu_layer), GColorVividCerulean, GColorWhite);
#endif
}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(main_menu_layer);
  window_destroy(window);
  s_main_window = NULL;
}

void menu_window_push() {
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }
  window_stack_push(s_main_window, true);
}
