#include <pebble.h>
#include "state.h"
#include "deps/list/list.h"

static const char OPPONENT_SCORE = 'O';
static const char PLAYER_SCORE = 'P';

State state_new(Settings *settings) {
  return (State)
    { .player_score = 0
    , .opponent_score = 0
    , .player_sets = 0
    , .opponent_sets = 0
    , .player_sets_final_scores = {0}
    , .opponent_sets_final_scores = {0}
    , .is_complete = false
    , .is_final_set = settings->num_sets == 1
    , .num_sets = settings->num_sets
    , .num_points = settings->num_points
    , .max_points = settings->max_points
    , .final_set = settings->final_set
    , .server = settings->first_server
    };
}

State compute_state(list_t *serial, Settings *settings) {
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(serial, LIST_HEAD);
  State state = state_new(settings);
  while ((node = list_iterator_next(it))) next_state(&state, (char *) node->val);
  list_iterator_destroy(it);
  return state;
}

void toggle_server(State *s, bool is_player_score) {
  // Toggle the server
  if (s->server == PLAYER && !is_player_score) {
    s->server = OPPONENT;
  } else if (s->server == OPPONENT && is_player_score) {
    s->server = PLAYER;
  }
}

void next_state(State *s, char *point) {

  bool is_player_score = *point == PLAYER_SCORE;
  int *scorer = is_player_score ? &s->player_score : &s->opponent_score;
  int *non_scorer = is_player_score ? &s->opponent_score : &s->player_score;
  int *scorer_sets = is_player_score ? &s->player_sets : &s->opponent_sets;
  int *non_scorer_sets = is_player_score ? &s->opponent_sets : &s->player_sets;

  increment_point(s, scorer, non_scorer, scorer_sets, non_scorer_sets, is_player_score);
}

void increment_point(State *s, int *scorer, int *non_scorer, int *scorer_sets, int *non_scorer_sets, bool is_player_score) {

    *scorer = *scorer + 1;
  
    toggle_server(s, is_player_score);
  
    if ((*scorer == s->num_points && *scorer - *non_scorer >= 2) // normal end
        || (*scorer > s->num_points && (*scorer - *non_scorer >= 2 || *scorer == s->max_points))) // + 2 end or max
    {
      // Save end score of set in state
      int current_set = *scorer_sets + *non_scorer_sets;
      s->player_sets_final_scores[current_set] = is_player_score ? *scorer : *non_scorer;
      s->opponent_sets_final_scores[current_set]= is_player_score ? *non_scorer : *scorer;
      
      // Reset scores to 0 and next state
      *scorer = 0;
      *non_scorer = 0;
      increment_set(s, scorer_sets, non_scorer_sets);
    }
}

void increment_set(State *s, int *scorer_sets, int *non_scorer_sets) {
  *scorer_sets = *scorer_sets + 1;  
  s->is_final_set = *scorer_sets + *non_scorer_sets == s->num_sets - 1;
  if (*scorer_sets > s->num_sets - *scorer_sets) s->is_complete = true;
}

void debug_state(State *s) {

  char player_score[4];
  char opponent_score[4];
  
  snprintf(player_score, 3, "%d", s->player_score);
  snprintf(opponent_score, 3, "%d", s->opponent_score);

  APP_LOG(APP_LOG_LEVEL_DEBUG
    , "%d set match, %s-%s, SETS: %d-%d, Server: %d, Final set: %d"
    , s->num_sets, player_score, opponent_score, s->player_sets, s->opponent_sets, 
      s->server, s->is_final_set);
}
