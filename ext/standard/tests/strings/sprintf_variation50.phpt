--TEST--
Test sprintf() function : usage variations - scientific formats with string values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : scientific formats with string values ***\n";

// array of string values 
$string_values = array(
  "",
  '',
  "0",
  '0',
  "1",
  '1',
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
  'null',
  "333333333333333333333333333333"
);

// array of scientific formats
$scientific_formats = array(
  "%e", "%he", "%le",
  "%Le", " %e", "%e ",
  "\t%e", "\n%e", "%4e",
  "%30e", "%[0-1]", "%*e"
);


$count = 1;
foreach($string_values as $string_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($scientific_formats as $format) {
    var_dump( sprintf($format, $string_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : scientific formats with string values ***

-- Iteration 1 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 2 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 3 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 4 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 5 --
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(12) " 1.000000e+0"
unicode(12) "1.000000e+0 "
unicode(12) "	1.000000e+0"
unicode(12) "
1.000000e+0"
unicode(11) "1.000000e+0"
unicode(30) "                   1.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 6 --
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(11) "1.000000e+0"
unicode(1) "e"
unicode(12) " 1.000000e+0"
unicode(12) "1.000000e+0 "
unicode(12) "	1.000000e+0"
unicode(12) "
1.000000e+0"
unicode(11) "1.000000e+0"
unicode(30) "                   1.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 7 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 8 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 9 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 10 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 11 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 12 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 13 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 14 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 15 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 16 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 17 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 18 --
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(11) "0.000000e+0"
unicode(1) "e"
unicode(12) " 0.000000e+0"
unicode(12) "0.000000e+0 "
unicode(12) "	0.000000e+0"
unicode(12) "
0.000000e+0"
unicode(11) "0.000000e+0"
unicode(30) "                   0.000000e+0"
unicode(4) "0-1]"
unicode(1) "e"

-- Iteration 19 --
unicode(12) "3.333333e+29"
unicode(1) "e"
unicode(12) "3.333333e+29"
unicode(1) "e"
unicode(13) " 3.333333e+29"
unicode(13) "3.333333e+29 "
unicode(13) "	3.333333e+29"
unicode(13) "
3.333333e+29"
unicode(12) "3.333333e+29"
unicode(30) "                  3.333333e+29"
unicode(4) "0-1]"
unicode(1) "e"
Done
