#ifndef _QUERY_SPARSER_H_
#define _QUERY_SPARSER_H_

#include "common.h"
#include "sparser.h"

typedef sparser_callback_t parser_t;

/** Uses sparser and RapidJSON to count the number of records matching the
 * predicate.
 *
 * @param filename the data to check
 * @param query a search query. If this is NULL, TEXT is used. At most four
 * bytes from this query will be used.
 * @param callback the callback which invokes the full parser.
 *
 * @return the running time.
 */
double bench_sparser(const char *filename, const char *query,
                     parser_t callback) {
  // Read in the data into a buffer.
  char *raw = NULL;
  long length = read_all(filename, &raw);

  bench_timer_t s = time_start();

  int query_length = strlen(query);
  if (query_length > 4) {
    query_length = 4;
  }

  sparser_stats_t *stats =
      sparser_search_single(raw, length, query, query_length, callback);

  assert(stats);

  double parse_time = time_stop(s);

  printf("%s\n", sparser_format_stats(stats));
  printf("Runtime: %f seconds\n", parse_time);

  free(raw);
  free(stats);

  return parse_time;
}

/* Times splitting the input by newline and calling the full parser on each line.
 *
 * @param filename
 * @param callback the function which performs the parse.
 */
double bench_rapidjson(const char *filename, parser_t callback) {
  char *data, *line;
  size_t bytes = read_all(filename, &data);
  int doc_index = 1;
  int matching = 0;

  bench_timer_t s = time_start();

  while ((line = strsep(&data, "\n")) != NULL) {
    if (callback(line)) {
      matching++;
    }
    doc_index++;
  }

  double elapsed = time_stop(s);
  printf("Passing Elements: %d of %d records (%.3f seconds)\n", matching,
         doc_index, elapsed);
  return elapsed;
}

#endif
