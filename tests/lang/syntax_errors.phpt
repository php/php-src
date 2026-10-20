--TEST--
Detailed reporting on specific types of syntax errors
--FILE--
<?php
$badCode = [
  "if(1 > 2",                   /* unclosed ( */
  "[1, 2,",                     /* unclosed [ */
  "if(1) { echo 'hello'; ",     /* unclosed { */
  "(1 + 2));",                  /* too many ) */
  "[1, 2]]",                    /* too many ] */
  "if (1) {  }  }",             /* too many } */
  "(1 + 2];",                   /* ] doesn't match ( */
  "[1, 2)];",                   /* ) doesn't match [ */
  "if(1) { echo 'a'; )}",       /* ) doesn't match { */
  /* separately test cases where the faulty construct spans multiple lines,
     since the error message should refer to the starting line in those cases */
  "if(1 > 2) {\n  echo '1';",   /* unclosed (, spans multiple lines */
  "[1,\n2,\n3,",                /* unclosed [, spans multiple lines */
  "{\n  echo '1';\n echo '2';", /* unclosed {, spans multiple lines */
  "(1 +\n 2 +\n 3))",           /* too many ), spans multiple lines */
  "[1,\n2,\n3]];",              /* too many ], spans multiple lines */
  "if (1)\n  {\n    }}",        /* too many }, spans multiple lines */
  "(1 +\n\n  2])",              /* ] doesn't match (, spans multiple lines */
  "[1,\n2,\n3)]",               /* ) doesn't match [, spans multiple lines */
  "if(1) {\n  echo 'a';\n)}",   /* ) doesn't match {, spans multiple lines */
  ];

foreach ($badCode as $code) {
  try {
    eval($code);
  } catch (ParseError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
  }
}

echo "==DONE==\n";
?>
--EXPECT--
ParseError: Unclosed '('
ParseError: Unclosed '['
ParseError: Unclosed '{'
ParseError: Unmatched ')'
ParseError: Unmatched ']'
ParseError: Unmatched '}'
ParseError: Unclosed '(' does not match ']'
ParseError: Unclosed '[' does not match ')'
ParseError: Unclosed '{' does not match ')'
ParseError: Unclosed '{' on line 1
ParseError: Unclosed '[' on line 1
ParseError: Unclosed '{' on line 1
ParseError: Unmatched ')'
ParseError: Unmatched ']'
ParseError: Unmatched '}'
ParseError: Unclosed '(' on line 1 does not match ']'
ParseError: Unclosed '[' on line 1 does not match ')'
ParseError: Unclosed '{' on line 1 does not match ')'
==DONE==
