/*
 * posix.c
 */
#include<stdio.h>
#include "onigposix.h"

static int x(regex_t* reg, unsigned char* pattern, unsigned char* str)
{
  int r, i;
  char buf[200];
  regmatch_t pmatch[20];

  r = regexec(reg, str, reg->re_nsub + 1, pmatch, 0);
  if (r != 0 && r != REG_NOMATCH) {
    regerror(r, reg, buf, sizeof(buf));
    fprintf(stderr, "ERROR: %s\n", buf);
    exit(-1);
  }

  if (r == REG_NOMATCH) {
    fprintf(stderr, "FAIL: /%s/ '%s'\n", pattern, str);
  }
  else {
    fprintf(stderr, "OK: /%s/ '%s'\n", pattern, str);
    for (i = 0; i <= reg->re_nsub; i++) {
      fprintf(stderr, "%d: %d-%d\n", i, pmatch[i].rm_so, pmatch[i].rm_eo);
    }
  }
  return 0;
}

extern int main(int argc, char* argv[])
{
  int r;
  char buf[200];
  regex_t reg;
  unsigned char* pattern;

  /* default syntax (REG_SYNTAX_RUBY) */
  pattern = "^a+b{2,7}[c-f]?$|uuu";
  r = regcomp(&reg, pattern, REG_EXTENDED);
  if (r) {
    regerror(r, &reg, buf, sizeof(buf));
    fprintf(stderr, "ERROR: %s\n", buf);
    exit(-1);
  }
  x(&reg, pattern, "aaabbbbd");

  /* POSIX Basic RE (REG_EXTENDED is not specified.) */
  pattern = "^a+b{2,7}[c-f]?|uuu";
  r = regcomp(&reg, pattern, 0);
  if (r) {
    regerror(r, &reg, buf, sizeof(buf));
    fprintf(stderr, "ERROR: %s\n", buf);
    exit(-1);
  }
  x(&reg, pattern, "a+b{2,7}d?|uuu");

  /* POSIX Basic RE (REG_EXTENDED is not specified.) */
  pattern = "^a*b\\{2,7\\}\\([c-f]\\)$";
  r = regcomp(&reg, pattern, 0);
  if (r) {
    regerror(r, &reg, buf, sizeof(buf));
    fprintf(stderr, "ERROR: %s\n", buf);
    exit(-1);
  }
  x(&reg, pattern, "aaaabbbbbbd");

  /* POSIX Extended RE */
  regex_set_default_syntax(REG_SYNTAX_POSIX_EXTENDED);
  pattern = "^a+b{2,7}[c-f]?)$|uuu";
  r = regcomp(&reg, pattern, REG_EXTENDED);
  if (r) {
    regerror(r, &reg, buf, sizeof(buf));
    fprintf(stderr, "ERROR: %s\n", buf);
    exit(-1);
  }
  x(&reg, pattern, "aaabbbbd)");

  pattern = "^b.";
  r = regcomp(&reg, pattern, REG_EXTENDED | REG_NEWLINE);
  if (r) {
    regerror(r, &reg, buf, sizeof(buf));
    fprintf(stderr, "ERROR: %s\n", buf);
    exit(-1);
  }
  x(&reg, pattern, "a\nb\n");

  regfree(&reg);
  regex_end();
  return 0;
}
