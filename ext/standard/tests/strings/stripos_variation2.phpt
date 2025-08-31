--TEST--
Test stripos() function : usage variations - single quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
/* Test stripos() function by passing single quoted strings to 'haystack' & 'needle' arguments */

echo "*** Testing stripos() function: with single quoted strings ***\n";
$haystack = 'Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 ';
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

  //boolean false
  FALSE,
  false,

  //empty string
  '',

  //special chars
  ' ',
  '$',
  ' $',
  '&',
  '!#',
  '%\o',
  '\o,',
  '()',
  '*+',
  '+',
  '-',
  '.',
  '.;',
  '.;',
  ':;',
  ';',
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
  var_dump( stripos($haystack, $needle[$index]) );
  var_dump( stripos($haystack, $needle[$index], $index) );
  $count++;
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: with single quoted strings ***
-- Iteration 1 --
int(2)
int(2)
-- Iteration 2 --
int(2)
int(2)
-- Iteration 3 --
int(0)
int(38)
-- Iteration 4 --
int(0)
int(38)
-- Iteration 5 --
int(6)
int(6)
-- Iteration 6 --
int(6)
int(6)
-- Iteration 7 --
bool(false)
bool(false)
-- Iteration 8 --
int(8)
int(8)
-- Iteration 9 --
int(8)
int(8)
-- Iteration 10 --
bool(false)
bool(false)
-- Iteration 11 --
int(10)
int(10)
-- Iteration 12 --
int(0)
int(11)
-- Iteration 13 --
int(0)
int(12)
-- Iteration 14 --
int(0)
int(13)
-- Iteration 15 --
int(14)
int(14)
-- Iteration 16 --
int(16)
int(16)
-- Iteration 17 --
int(15)
bool(false)
-- Iteration 18 --
int(17)
int(17)
-- Iteration 19 --
int(18)
int(18)
-- Iteration 20 --
int(20)
int(20)
-- Iteration 21 --
int(21)
int(21)
-- Iteration 22 --
int(24)
int(24)
-- Iteration 23 --
int(26)
int(26)
-- Iteration 24 --
int(27)
int(27)
-- Iteration 25 --
int(28)
int(28)
-- Iteration 26 --
int(29)
int(29)
-- Iteration 27 --
bool(false)
bool(false)
-- Iteration 28 --
bool(false)
bool(false)
-- Iteration 29 --
int(31)
int(31)
-- Iteration 30 --
int(32)
int(32)
-- Iteration 31 --
int(33)
int(33)
-- Iteration 32 --
int(35)
int(35)
-- Iteration 33 --
int(34)
int(34)
-- Iteration 34 --
int(36)
int(36)
-- Iteration 35 --
int(37)
int(37)
-- Iteration 36 --
int(37)
int(37)
-- Iteration 37 --
int(43)
int(43)
-- Iteration 38 --
int(52)
int(52)
-- Iteration 39 --
int(19)
bool(false)
-- Iteration 40 --
int(58)
int(58)
-- Iteration 41 --
bool(false)
bool(false)
-- Iteration 42 --
bool(false)
bool(false)
-- Iteration 43 --
bool(false)
bool(false)
-- Iteration 44 --
int(0)
bool(false)
*** Done ***
