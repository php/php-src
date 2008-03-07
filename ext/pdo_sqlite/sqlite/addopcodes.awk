#!/usr/bin/awk
#
# This script appends additional token codes to the end of the
# parse.h file that lemon generates.  These extra token codes are
# not used by the parser.  But they are used by the tokenizer and/or
# the code generator.
#
#
BEGIN {
  max = 0
}
/^#define TK_/ {
  print $0
  if( max<$3 ) max = $3
}
END {
  printf "#define TK_%-29s %4d\n", "TO_TEXT",         max+1
  printf "#define TK_%-29s %4d\n", "TO_BLOB",         max+2
  printf "#define TK_%-29s %4d\n", "TO_NUMERIC",      max+3
  printf "#define TK_%-29s %4d\n", "TO_INT",          max+4
  printf "#define TK_%-29s %4d\n", "TO_REAL",         max+5
  printf "#define TK_%-29s %4d\n", "END_OF_FILE",     max+6
  printf "#define TK_%-29s %4d\n", "ILLEGAL",         max+7
  printf "#define TK_%-29s %4d\n", "SPACE",           max+8
  printf "#define TK_%-29s %4d\n", "UNCLOSED_STRING", max+9
  printf "#define TK_%-29s %4d\n", "COMMENT",         max+10
  printf "#define TK_%-29s %4d\n", "FUNCTION",        max+11
  printf "#define TK_%-29s %4d\n", "COLUMN",          max+12
  printf "#define TK_%-29s %4d\n", "AGG_FUNCTION",    max+13
  printf "#define TK_%-29s %4d\n", "AGG_COLUMN",      max+14
  printf "#define TK_%-29s %4d\n", "CONST_FUNC",      max+15
}
