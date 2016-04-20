#include <pebble.h>
#include "serial.h"
#include "../deps/list/list.h"

list_t *serial_new() {
  return list_new();
}

void add_opponent_score(list_t *serial) {
  list_rpush(serial, list_node_new((void *) OPPONENT_SCORE));
}

void add_player_score(list_t *serial) {
  list_rpush(serial, list_node_new((void *) PLAYER_SCORE));
}

void undo(list_t *serial) {
  free(list_rpop(serial));
}
