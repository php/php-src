--TEST--
Test strrchr() function : usage variations - single quoted strings
--FILE--
<?php
/* Test strrchr() function by passing various single quoted strings to 'haystack' & 'needle' */

echo "*** Testing strrchr() function: with various single quoted strings ***";
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
  NULL,
  null,

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
  echo "\n-- Iteration $count --\n";
  var_dump( strrchr($haystack, $needle[$index]) );
  $count ++;
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrchr() function: with various single quoted strings ***
-- Iteration 1 --
string(22) "lo123456he \x234 \101 "

-- Iteration 2 --
bool(false)

-- Iteration 3 --
string(63) "Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 4 --
string(14) "he \x234 \101 "

-- Iteration 5 --
string(5) "\101 "

-- Iteration 6 --
string(5) "\101 "

-- Iteration 7 --
string(1) " "

-- Iteration 8 --
string(5) "\101 "

-- Iteration 9 --
string(5) "\101 "

-- Iteration 10 --
bool(false)

-- Iteration 11 --
string(5) "\101 "

-- Iteration 12 --
bool(false)

-- Iteration 13 --
bool(false)

-- Iteration 14 --
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
string(1) " "

-- Iteration 18 --
string(47) "$&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 19 --
string(1) " "

-- Iteration 20 --
string(46) "&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 21 --
string(45) "!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 22 --
string(43) "%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 23 --
string(5) "\101 "

-- Iteration 24 --
string(39) "()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 25 --
string(37) "*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 26 --
string(36) "+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 27 --
string(35) "-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 28 --
string(34) "./:;<=>?@hello123456he \x234 \101 "

-- Iteration 29 --
string(34) "./:;<=>?@hello123456he \x234 \101 "

-- Iteration 30 --
string(32) ":;<=>?@hello123456he \x234 \101 "

-- Iteration 31 --
string(31) ";<=>?@hello123456he \x234 \101 "

-- Iteration 32 --
string(30) "<=>?@hello123456he \x234 \101 "

-- Iteration 33 --
string(28) ">?@hello123456he \x234 \101 "

-- Iteration 34 --
string(29) "=>?@hello123456he \x234 \101 "

-- Iteration 35 --
string(27) "?@hello123456he \x234 \101 "

-- Iteration 36 --
string(26) "@hello123456he \x234 \101 "

-- Iteration 37 --
string(26) "@hello123456he \x234 \101 "

-- Iteration 38 --
string(2) "1 "

-- Iteration 39 --
string(5) "\101 "

-- Iteration 40 --
string(44) "#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 41 --
string(5) "\101 "

-- Iteration 42 --
bool(false)

-- Iteration 43 --
string(7) "4 \101 "

-- Iteration 44 --
string(7) "4 \101 "

-- Iteration 45 --
string(63) "Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "
*** Done ***
