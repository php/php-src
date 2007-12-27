--TEST--
Test sprintf() function : usage variations - unsigned formats with string values
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : unsigned formats with string values ***\n";

// array of string values 
$string_values = array(
  "",
  '',
  "0",
  '0',
  "1",
  '1',
  "-123",
  "+123",
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

// array of unsigned formats
$unsigned_formats = array( 
  "%u", "%hu", "%lu",
  "%Lu", " %u", "%u ",   
  "\t%u", "\n%u", "%4u", 
   "%30u", "%[0-9]", "%*u"
);


$count = 1;
foreach($string_values as $string_value) {
  echo "\n-- Iteration $count --\n";
  
  foreach($unsigned_formats as $format) {
    var_dump( sprintf($format, $string_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : unsigned formats with string values ***

-- Iteration 1 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 2 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 3 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 4 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 5 --
string(1) "1"
string(1) "u"
string(1) "1"
string(1) "u"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "u"

-- Iteration 6 --
string(1) "1"
string(1) "u"
string(1) "1"
string(1) "u"
string(2) " 1"
string(2) "1 "
string(2) "	1"
string(2) "
1"
string(4) "   1"
string(30) "                             1"
string(4) "0-9]"
string(1) "u"

-- Iteration 7 --
string(20) "18446744073709551493"
string(1) "u"
string(20) "18446744073709551493"
string(1) "u"
string(21) " 18446744073709551493"
string(21) "18446744073709551493 "
string(21) "	18446744073709551493"
string(21) "
18446744073709551493"
string(20) "18446744073709551493"
string(30) "          18446744073709551493"
string(4) "0-9]"
string(1) "u"

-- Iteration 8 --
string(3) "123"
string(1) "u"
string(3) "123"
string(1) "u"
string(4) " 123"
string(4) "123 "
string(4) "	123"
string(4) "
123"
string(4) " 123"
string(30) "                           123"
string(4) "0-9]"
string(1) "u"

-- Iteration 9 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 10 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 11 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 12 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 13 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 14 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 15 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 16 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 17 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 18 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 19 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"

-- Iteration 20 --
string(1) "0"
string(1) "u"
string(1) "0"
string(1) "u"
string(2) " 0"
string(2) "0 "
string(2) "	0"
string(2) "
0"
string(4) "   0"
string(30) "                             0"
string(4) "0-9]"
string(1) "u"
Done

--UEXPECTF--
*** Testing sprintf() : unsigned formats with string values ***

-- Iteration 1 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 2 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 3 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 4 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 5 --
unicode(1) "1"
unicode(1) "u"
unicode(1) "1"
unicode(1) "u"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 6 --
unicode(1) "1"
unicode(1) "u"
unicode(1) "1"
unicode(1) "u"
unicode(2) " 1"
unicode(2) "1 "
unicode(2) "	1"
unicode(2) "
1"
unicode(4) "   1"
unicode(30) "                             1"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 7 --
unicode(20) "18446744073709551493"
unicode(1) "u"
unicode(20) "18446744073709551493"
unicode(1) "u"
unicode(21) " 18446744073709551493"
unicode(21) "18446744073709551493 "
unicode(21) "	18446744073709551493"
unicode(21) "
18446744073709551493"
unicode(20) "18446744073709551493"
unicode(30) "          18446744073709551493"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 8 --
unicode(3) "123"
unicode(1) "u"
unicode(3) "123"
unicode(1) "u"
unicode(4) " 123"
unicode(4) "123 "
unicode(4) "	123"
unicode(4) "
123"
unicode(4) " 123"
unicode(30) "                           123"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 9 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 10 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 11 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 12 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 13 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 14 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 15 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 16 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 17 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 18 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 19 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"

-- Iteration 20 --
unicode(1) "0"
unicode(1) "u"
unicode(1) "0"
unicode(1) "u"
unicode(2) " 0"
unicode(2) "0 "
unicode(2) "	0"
unicode(2) "
0"
unicode(4) "   0"
unicode(30) "                             0"
unicode(4) "0-9]"
unicode(1) "u"
Done
