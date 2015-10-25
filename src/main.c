#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_question_layer, *s_prompt_layer;

static DictationSession *s_dictation_session;

static char s_last_text[256];

static bool s_speaking_enabled;

static void sendMessage(char *last_text){
  DictionaryIterator* dictionaryIterator = NULL;
  app_message_outbox_begin(&dictionaryIterator);
  dict_write_cstring(dictionaryIterator,0,last_text);
  dict_write_end(dictionaryIterator);
  app_message_outbox_send();
}

static void recieveMessage(char *last_text){
  DictionaryIterator* dictionaryIterator = NULL;
  app_message_outbox_begin(&dictionaryIterator);
  dict_write_cstring(dictionaryIterator,0,last_text);
  dict_write_end(dictionaryIterator);
  app_message_outbox_send();
}

static void next_def_handler(void *word) {
  //text_layer_set_text(s_question_layer, word);
  //text_layer_set_text(s_prompt_layer, "Press Select to search your word!");
  window_set_background_color(s_main_window, GColorVividCerulean );
  s_speaking_enabled = true;
}

static void check_def(char *word) {
  //get the def from JS
  snprintf(s_last_text, sizeof(s_last_text), "%s", word);
  sendMessage(s_last_text);
  text_layer_set_text(s_question_layer, s_last_text);
  text_layer_set_text(s_prompt_layer, "DEF GOES HERE");
  app_timer_register(3000, next_def_handler, NULL);
}

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, 
                                       char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    // Check for the def 
    check_def(transcription);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Transcription failed.\n\nError ID:\n%d", (int)status);
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_speaking_enabled) {
    // Start voice dictation UI
    dictation_session_start(s_dictation_session);
    s_speaking_enabled = false;
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_question_layer = text_layer_create(GRect(5, 5, bounds.size.w - 10, bounds.size.h));
  text_layer_set_font(s_question_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_question_layer, GColorWhite);
  text_layer_set_text_alignment(s_question_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_question_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_question_layer));
  
  s_prompt_layer = text_layer_create(GRect(5, 120, bounds.size.w - 10, bounds.size.h));
  text_layer_set_text(s_prompt_layer, "Press Select to search your word!");
  text_layer_set_font(s_prompt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_prompt_layer, GColorWhite);
  text_layer_set_text_alignment(s_prompt_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_prompt_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_prompt_layer));
 
}

static void window_unload(Window *window) {
  text_layer_destroy(s_prompt_layer);
  text_layer_destroy(s_question_layer);
}

static void init() {
  app_message_open(512, 512);
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  // Create new dictation session
  s_dictation_session = dictation_session_create(sizeof(s_last_text), 
                                                 dictation_session_callback, NULL);

  window_set_background_color(s_main_window, GColorVividCerulean);

  text_layer_set_text(s_question_layer, "PebbleDict");
  s_speaking_enabled = true;
}

static void deinit() {
  // Free the last session data
  dictation_session_destroy(s_dictation_session);

  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}