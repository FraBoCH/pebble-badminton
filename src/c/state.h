#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include "serial.h"
#include "deps/list/list.h"

typedef struct {
  int player_score;
  int opponent_score;
  int player_sets;
  int opponent_sets;
  int player_sets_final_scores[3];
  int opponent_sets_final_scores[3];
  bool is_complete;
  bool is_final_set;
  int num_sets;
  int num_points;
  int max_points;
  int final_set;
  int server;
} State;

typedef enum {
  PLAYER=0,
  OPPONENT=1
} SERVER;

typedef struct {
  int num_sets;
  int num_points;
  int max_points;
  int final_set;
  int first_server;
} Settings;

State compute_state(list_t *serial, Settings* settings);
void next_state(State *s, char *point);
void debug_state(State *s);
void increment_point(State *s, int *scorer, int *non_scorer, int *scorer_sets, int *non_scorer_sets, bool is_player_score);
void increment_set(State *s, int *scorer_sets, int *non_scorer_sets);

#endif
