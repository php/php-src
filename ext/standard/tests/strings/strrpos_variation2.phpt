--TEST--
Test strrpos() function : usage variations - single quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
/* Test strrpos() function by passing single quoted strings to 'haystack' & 'needle' arguments */

echo "*** Testing strrpos() function: with single quoted strings ***\n";
$haystack = 'Hello,\t\n\0\n  $&!#%()*<=>?@hello123456he \x234 \101 ';
$needle = array(
  //regular strings
  'l',
  'L',
  'HELLO',
  'hEllo',

  //escape characters
  '\t',
  '\T',
  '     ',
  '\n',
  '\N',
  '
',  //new line

  //nulls
  '\0',

  //empty string
  '',

  //special chars
  ' ',
  '$',
  ' $',
  '&',
  '!#',
  '()',
  '<=>',
  '>',
  '=>',
  '?',
  '@',
  '@hEllo',

  '12345', //decimal numeric string
  '\x23',  //hexadecimal numeric string
  '#',  //hexadecimal numeric string
  '\101',  //octal numeric string
  'A',
  '456HEE',  //numerics + chars
  42, //needle as int(ASCII value of '*')
  $haystack  //haystack as needle
);

/* loop through to get the position of the needle in haystack string */
$count = 1;
for($index=0; $index<count($needle); $index++) {
  echo "-- Iteration $count --\n";
  var_dump( strrpos($haystack, $needle[$index]) );
  var_dump( strrpos($haystack, $needle[$index], $index) );
  $count++;
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function: with single quoted strings ***
-- Iteration 1 --
int(32)
int(32)
-- Iteration 2 --
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
-- Iteration 4 --
bool(false)
bool(false)
-- Iteration 5 --
int(6)
int(6)
-- Iteration 6 --
bool(false)
bool(false)
-- Iteration 7 --
bool(false)
bool(false)
-- Iteration 8 --
int(12)
int(12)
-- Iteration 9 --
bool(false)
bool(false)
-- Iteration 10 --
bool(false)
bool(false)
-- Iteration 11 --
int(10)
int(10)
-- Iteration 12 --
int(54)
int(54)
-- Iteration 13 --
int(53)
int(53)
-- Iteration 14 --
int(16)
int(16)
-- Iteration 15 --
int(15)
int(15)
-- Iteration 16 --
int(17)
int(17)
-- Iteration 17 --
int(18)
int(18)
-- Iteration 18 --
int(21)
int(21)
-- Iteration 19 --
int(24)
int(24)
-- Iteration 20 --
int(26)
int(26)
-- Iteration 21 --
int(25)
int(25)
-- Iteration 22 --
int(27)
int(27)
-- Iteration 23 --
int(28)
int(28)
-- Iteration 24 --
bool(false)
bool(false)
-- Iteration 25 --
int(34)
int(34)
-- Iteration 26 --
int(43)
int(43)
-- Iteration 27 --
int(19)
bool(false)
-- Iteration 28 --
int(49)
int(49)
-- Iteration 29 --
bool(false)
bool(false)
-- Iteration 30 --
bool(false)
bool(false)
-- Iteration 31 --
bool(false)
bool(false)
-- Iteration 32 --
int(0)
bool(false)
*** Done ***
