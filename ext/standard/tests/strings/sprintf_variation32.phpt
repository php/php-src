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
--EXPECT--
*** Testing sprintf() : octal formats with string values ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 2 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 3 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 4 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 5 --
unicode(1) "1"
unicode(1) "o"
unicode(1) "1"
unicode(1) "o"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 6 --
unicode(1) "1"
unicode(1) "o"
unicode(1) "1"
unicode(1) "o"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 7 --
unicode(3) "173"
unicode(1) "o"
unicode(3) "173"
unicode(1) "o"
unicode(4) " 173"
unicode(4) "173 "
unicode(4) "	173"
unicode(4) "
173"
unicode(4) " 173"
unicode(30) "                           173"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 8 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 9 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 10 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 11 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 12 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 13 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 14 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 15 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 16 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 17 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 18 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"

-- Iteration 19 --
unicode(1) "0"
unicode(1) "o"
unicode(1) "0"
unicode(1) "o"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-7]"
unicode(1) "o"
Done
