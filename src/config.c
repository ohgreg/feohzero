#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "moves.h"

/* handler function type. returns pointer to next argv element
 * to process, or NULL on error */
typedef char **(*arg_handler)(char **argv, Config *config, const char **error);

/* argument option structure */
typedef struct {
  const char *name;
  arg_handler handler;
  const char *desc;
} Arg;

/* parses an integer from a string with validation. returns NULL on success,
 * error message on failure */
static const char *parse_int(const char *str, int *out, int min_val,
                             int max_val) {
  static char err_buf[256];

  if (str == NULL || *str == '\0') {
    return "empty value provided";
  }

  char *endptr;
  long val = strtol(str, &endptr, 10);

  if (*endptr != '\0') {
    return "not a valid integer";
  }

  if (val < min_val) {
    snprintf(err_buf, sizeof(err_buf), "must be at least %d", min_val);
    return err_buf;
  }

  if (val > max_val) {
    snprintf(err_buf, sizeof(err_buf), "must be at most %d", max_val);
    return err_buf;
  }

  *out = (int)val;
  return NULL;
}

/* handler for --moves argument */
static char **handle_moves(char **argv, Config *config, const char **error) {
  if (*(argv + 1) == NULL) {
    *error = "requires an argument";
    return NULL;
  }

  if (**(argv + 1) == '\0') {
    *error = "moves string cannot be empty";
    return NULL;
  }

  config->moves = *(argv + 1);
  return argv + 2;
}

/* handler for --notation argument */
static char **handle_notation(char **argv, Config *config, const char **error) {
  if (*(argv + 1) == NULL) {
    *error = "requires an argument (san or uci)";
    return NULL;
  }

  char *notation = *(argv + 1);

  if (strcmp(notation, "san") == 0) {
    config->move_to_str = move_to_san;
    config->str_to_move = san_to_move;
  } else if (strcmp(notation, "uci") == 0) {
    config->move_to_str = move_to_uci;
    config->str_to_move = uci_to_move;
  } else {
    *error = "must be 'san' or 'uci'";
    return NULL;
  }

  return argv + 2;
}

/* handler for --timeout argument */
static char **handle_timeout(char **argv, Config *config, const char **error) {
  if (*(argv + 1) == NULL) {
    *error = "requires an argument";
    return NULL;
  }

  const char *err = parse_int(*(argv + 1), &config->timeout, 10, 30000);
  if (err != NULL) {
    *error = err;
    return NULL;
  }

  return argv + 2;
}

/* handler for --depth argument */
static char **handle_depth(char **argv, Config *config, const char **error) {
  if (*(argv + 1) == NULL) {
    *error = "requires an argument";
    return NULL;
  }

  const char *err = parse_int(*(argv + 1), &config->depth, 1, 100);
  if (err != NULL) {
    *error = err;
    return NULL;
  }

  return argv + 2;
}

/* handler for --tt-size argument */
static char **handle_tt_size(char **argv, Config *config, const char **error) {
  if (*(argv + 1) == NULL) {
    *error = "requires an argument";
    return NULL;
  }

  const char *err = parse_int(*(argv + 1), &config->tt_size_mb, 0, 64);
  if (err != NULL) {
    *error = err;
    return NULL;
  }

  return argv + 2;
}

/* handler for --seed argument */
static char **handle_seed(char **argv, Config *config, const char **error) {
  if (*(argv + 1) == NULL) {
    *error = "requires an argument";
    return NULL;
  }

  const char *err = parse_int(*(argv + 1), &config->seed, 0, INF);
  if (err != NULL) {
    *error = err;
    return NULL;
  }

  return argv + 2;
}

/* handler for --quiet argument */
static char **handle_quiet(char **argv, Config *config, const char **error) {
  (void)error;
  config->quiet = 1;
  return argv + 1;
}

/* handler for --help argument */
static char **handle_help(char **argv, Config *config, const char **error) {
  (void)error;
  config->help = 1;
  return argv + 1;
}

/* lookup table for all args */
static Arg options[] = {
    {"--moves", handle_moves,
     "moves in algebraic notation seperated by spaces"},
    {"--depth", handle_depth,
     "maximum search depth (default: " XSTR(DEFAULT_DEPTH) ")"},
    {"--timeout", handle_timeout,
     "search time limit in ms (default: " XSTR(DEFAULT_TIMEOUT) ")"},
    {"--notation", handle_notation,
     "move notation format, 'san' or 'uci' (default: san)"},
    {"--tt-size", handle_tt_size,
     "transposition table size in MB (default: " XSTR(DEFAULT_TT_SIZE_MB) ")"},
    {"--seed", handle_seed, "seed for RNG (default: " XSTR(DEFAULT_SEED) ")"},
    {"--quiet", handle_quiet, "only output the final best move"},
    {"--help", handle_help, "show this help message"},
    {NULL, NULL, NULL}};

/* prints usage information */
static void print_usage(const char *progname) {
  fprintf(stderr, "FeohZero v1.0 - by ohgreg & ItsFeold\n\n");
  fprintf(stderr, "usage: %s [fen] [options]\n\n", progname);
  fprintf(stderr,
          "  %-20s board state in FEN format (default: starting position)\n\n",
          "fen");
  fprintf(stderr, "options:\n");
  for (int i = 0; options[i].name != NULL; i++) {
    fprintf(stderr, "  %-20s %s\n", options[i].name, options[i].desc);
  }
}

void init_config(Config *config) {
  config->fen = NULL;
  config->move_to_str = move_to_san;
  config->str_to_move = san_to_move;
  config->moves = NULL;
  config->timeout = DEFAULT_TIMEOUT;
  config->depth = DEFAULT_DEPTH;
  config->tt_size_mb = DEFAULT_TT_SIZE_MB;
  config->seed = DEFAULT_SEED;
  config->quiet = 0;
  config->help = 0;
}

int parse_args(char *argv[], Config *config) {
  // parse all arguments
  char **curr = &argv[1];
  while (*curr != NULL) {
    int found = 0;
    for (int j = 0; options[j].name != NULL; j++) {
      if (strcmp(*curr, options[j].name) == 0) {
        found = 1;
        const char *err = NULL;
        char **next = options[j].handler(curr, config, &err);

        if (next == NULL) {
          fprintf(stderr, "Error! %s: %s\n", options[j].name, err);
          return 0;
        }

        if (config->help) {
          print_usage(argv[0]);
          return 0;
        }

        curr = next;
        break;
      }
    }

    if (!found) {
      // not an option, treat as FEN string
      if (config->fen != NULL) {
        fprintf(stderr, "Error! multiple FEN strings provided: '%s' and '%s'\n",
                config->fen, *curr);
        return 0;
      }
      config->fen = *curr;
      curr++;
    }
  }

  // start with start position if FEN was not provided
  if (config->fen == NULL) {
    config->fen = DEFAULT_FEN;
  }

  return 1;
}
