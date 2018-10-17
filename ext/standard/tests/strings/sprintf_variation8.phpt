--TEST--
Test sprintf() function : usage variations - int formats with string values
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : integer formats with string values ***\n";

// array of strings used to test the function
$string_values = array (
  "",
  '',
  "0",
  '0',
  "1",
  '1',
  "-123abc",
  "123abc",
  "+123abc",
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

// various integer formats
$int_formats = array(
  "%d", "%hd", "%ld",
  "%Ld", " %d", "%d ",
  "\t%d", "\n%d", "%4d",
  "%30d", "%[0-9]", "%*d"
);

$count = 1;
foreach($string_values as $string_value) {
  echo "\n-- Iteration $count --\n";

  foreach($int_formats as $format) {
    var_dump( sprintf($format, $string_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECT--
*** Testing sprintf() : integer formats with string values ***

-- Iteration 1 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 2 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 3 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 4 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 5 --
string(1) "1"
string(1) "d"
string(1) "1"
string(1) "d"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "d"

-- Iteration 6 --
string(1) "1"
string(1) "d"
string(1) "1"
string(1) "d"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "d"

-- Iteration 7 --
string(4) "-123"
string(1) "d"
string(4) "-123"
string(1) "d"
string(5) " -123"
string(5) "-123 "
string(5) "	-123"
string(5) "
-123"
string(4) "-123"
string(30) "                          -123"
string(4) "0-9]"
string(1) "d"

-- Iteration 8 --
string(3) "123"
string(1) "d"
string(3) "123"
string(1) "d"
string(4) " 123"
string(4) "123 "
string(4) "	123"
string(4) "
123"
string(4) " 123"
string(30) "                           123"
string(4) "0-9]"
string(1) "d"

-- Iteration 9 --
string(3) "123"
string(1) "d"
string(3) "123"
string(1) "d"
string(4) " 123"
string(4) "123 "
string(4) "	123"
string(4) "
123"
string(4) " 123"
string(30) "                           123"
string(4) "0-9]"
string(1) "d"

-- Iteration 10 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 11 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 12 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 13 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 14 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 15 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 16 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 17 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 18 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 19 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 20 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"

-- Iteration 21 --
string(1) "0"
string(1) "d"
string(1) "0"
string(1) "d"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "d"
Done
