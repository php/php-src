/*
 * simple.c
 */
#include<stdio.h>
#include "oniguruma.h"

extern int main(int argc, char* argv[])
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  RegErrorInfo einfo;
  RegRegion *region;

  static unsigned char* pattern = "a(.*)b|[e-f]+";
  static unsigned char* str = "zzzzaffffffffb";

  r = regex_new(&reg, pattern, pattern + strlen(pattern),
	REG_OPTION_DEFAULT, REGCODE_ASCII, REG_SYNTAX_DEFAULT, &einfo);
  if (r != REG_NORMAL) {
    char s[REG_MAX_ERROR_MESSAGE_LEN];
    regex_error_code_to_str(s, r, &einfo);
    fprintf(stderr, "ERROR: %s\n", s);
    exit(-1);
  }

  region = regex_region_new();

  end   = str + strlen(str);
  start = str;
  range = end;
  r = regex_search(reg, str, end, start, range, region, REG_OPTION_NONE);
  if (r >= 0) {
    int i;

    fprintf(stderr, "match at %d\n", r);
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stderr, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
  }
  else if (r == REG_MISMATCH) {
    fprintf(stderr, "search fail\n");
  }
  else { /* error */
    char s[REG_MAX_ERROR_MESSAGE_LEN];
    regex_error_code_to_str(s, r);
    exit(-1);
  }

  regex_region_free(region, 1 /* 1:free self, 0:free contents only */);
  regex_free(reg);
  regex_end();
  return 0;
}
