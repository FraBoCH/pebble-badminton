#include "summary.h"

static Window *s_main_window;
static State *final_state;

static TextLayer *result;
static TextLayer *match_score;
static TextLayer *match_score_details;
static char match_score_str[6];
static char match_score_details_str[30];
static char app_glance_message[64];

static void window_load(Window *window) {  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  result = text_layer_create(GRect(10, 10, bounds.size.w - 20, 38));
  char * result_str = final_state->player_sets > final_state->opponent_sets ? "You win!" : "You lose";
  text_layer_set_text(result, result_str);
#ifdef PBL_COLOR
  if (final_state->player_sets > final_state->opponent_sets) {
    text_layer_set_background_color(result, GColorJaegerGreen);
  } else {
    text_layer_set_background_color(result, GColorOrange);
  } 
#else
  text_layer_set_background_color(result, GColorOrange);
#endif 
  text_layer_set_text_color(result, GColorWhite);
  text_layer_set_font(result, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(result, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) result);

  match_score = text_layer_create(GRect(0, 55, bounds.size.w, 50));
  snprintf(match_score_str, sizeof(match_score_str), "%d - %d", final_state->player_sets, final_state->opponent_sets);
  text_layer_set_text(match_score, match_score_str);
  text_layer_set_font(match_score, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(match_score, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) match_score);
  
  match_score_details = text_layer_create(GRect(0, 115, bounds.size.w, 50));
  switch (final_state->player_sets + final_state->opponent_sets) {
    case 3:
      snprintf(match_score_details_str, sizeof(match_score_details_str), "%d - %d ; %d - %d ; %d - %d", 
                   final_state->player_sets_final_scores[0], final_state->opponent_sets_final_scores[0],
                   final_state->player_sets_final_scores[1], final_state->opponent_sets_final_scores[1],
                   final_state->player_sets_final_scores[2], final_state->opponent_sets_final_scores[2]);
      break;
    case 2:
      snprintf(match_score_details_str, sizeof(match_score_details_str), "%d - %d ; %d - %d", 
                   final_state->player_sets_final_scores[0], final_state->opponent_sets_final_scores[0],
                   final_state->player_sets_final_scores[1], final_state->opponent_sets_final_scores[1]);
      break;
    default:
      snprintf(match_score_details_str, sizeof(match_score_details_str), "%d - %d", 
                   final_state->player_sets_final_scores[0], final_state->opponent_sets_final_scores[0]);
      break;
  }                                                                                                   
  text_layer_set_text(match_score_details, match_score_details_str);
  text_layer_set_font(match_score_details, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(match_score_details, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) match_score_details);
  
  /* Compute app glance message */
  snprintf(app_glance_message, 64, "%s: %s (%s)", result_str, match_score_str, match_score_details_str);
}

static void prv_update_app_glance(AppGlanceReloadSession *session,
                                       size_t limit, void *context) {
  if (limit < 1) return;

  // Cast the context object to a string
  const char *message = context;

  // Create the AppGlanceSlice
  const AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .icon = PUBLISHED_ID_MENU_ICON,
      .subtitle_template_string = message
    },
    .expiration_time = APP_GLANCE_SLICE_NO_EXPIRATION
  };

  // Add the slice, and check the result
  const AppGlanceResult result = app_glance_add_slice(session, entry);

  if (result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "AppGlance Error: %d", result);
  }
}

static void menu_click_handler(ClickRecognizerRef recognize, void *context) {
  window_stack_pop(true);
  menu_window_push();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, menu_click_handler);
}

static void window_unload(Window *window) {
  text_layer_destroy(result);
  text_layer_destroy(match_score);
  text_layer_destroy(match_score_details);
  window_destroy(window);
  
  s_main_window = NULL;
  
  // Update app glance with last message before exiting
  app_glance_reload(prv_update_app_glance, app_glance_message);
}

void summary_window_push(State *s) {
  final_state = s;
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
    window_set_click_config_provider(s_main_window, click_config_provider);
  }
  window_stack_push(s_main_window, true);
}
