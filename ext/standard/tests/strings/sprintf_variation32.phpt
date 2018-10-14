--TEST--
Test sprintf() function : usage variations - octal formats with string values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : octal formats with string values ***\n";

// array of string values
$string_values = array(
  "",
  '',
  "0",
  '0',
  "1",
  '1',
  "0123",
  "\x01",
  '\x01',
  "\01",
  '\01',
  'string',
  "string",
  "true",
  "FALSE",
  'false',
  'TRUE',
  "NULL",
  'null'
);

// array of octal formats
$octal_formats = array(
  "%o", "%ho", "%lo",
  "%Lo", " %o", "%o ",
  "\t%o", "\n%o", "%4o",
  "%30o", "%[0-7]", "%*o"
);

$count = 1;
foreach($string_values as $string_value) {
  echo "\n-- Iteration $count --\n";

  foreach($octal_formats as $format) {
    var_dump( sprintf($format, $string_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : octal formats with string values ***

-- Iteration 1 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 2 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 3 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 4 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 5 --
string(1) "1"
string(1) "o"
string(1) "1"
string(1) "o"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-7]"
string(1) "o"

-- Iteration 6 --
string(1) "1"
string(1) "o"
string(1) "1"
string(1) "o"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-7]"
string(1) "o"

-- Iteration 7 --
string(3) "173"
string(1) "o"
string(3) "173"
string(1) "o"
string(4) " 173"
string(4) "173 "
string(4) "	173"
string(4) "
173"
string(4) " 173"
string(30) "                           173"
string(4) "0-7]"
string(1) "o"

-- Iteration 8 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 9 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 10 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 11 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 12 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 13 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 14 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 15 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 16 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 17 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 18 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"

-- Iteration 19 --
string(1) "0"
string(1) "o"
string(1) "0"
string(1) "o"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-7]"
string(1) "o"
Done
