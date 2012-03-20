#!/bin/sed -f
/^int yydebug;/{
i\
#ifndef yydebug
a\
#endif
}
/^yydestruct.*yymsg/,/#endif/{
  /^yydestruct/{
    /parser/!{
      h
      s/^/ruby_parser_&/
      s/)$/, parser)/
      /\*/s/parser)$/struct parser_params *&/
    }
  }
  /^#endif/{
    x
    /^./{
      i\
    struct parser_params *parser;
      a\
#define yydestruct(m, t, v) ruby_parser_yydestruct(m, t, v, parser)
    }
    x
  }
}
s/^\([ 	]*\)\(yyerror[ 	]*([ 	]*parser,\)/\1parser_\2/
s!^ *extern char \*getenv();!/* & */!
s/^\(#.*\)".*\.tab\.c"/\1"parse.c"/
/^\(#.*\)".*\.y"/s:\\\\:/:g
