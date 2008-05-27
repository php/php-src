--TEST--
Test strrchr() function : usage variations - single quoted strings
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

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
unicode(22) "lo123456he \x234 \101 "

-- Iteration 2 --
bool(false)

-- Iteration 3 --
unicode(63) "Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 4 --
unicode(14) "he \x234 \101 "

-- Iteration 5 --
unicode(5) "\101 "

-- Iteration 6 --
unicode(5) "\101 "

-- Iteration 7 --
unicode(1) " "

-- Iteration 8 --
unicode(5) "\101 "

-- Iteration 9 --
unicode(5) "\101 "

-- Iteration 10 --
bool(false)

-- Iteration 11 --
unicode(5) "\101 "

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
unicode(1) " "

-- Iteration 18 --
unicode(47) "$&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 19 --
unicode(1) " "

-- Iteration 20 --
unicode(46) "&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 21 --
unicode(45) "!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 22 --
unicode(43) "%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 23 --
unicode(5) "\101 "

-- Iteration 24 --
unicode(39) "()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 25 --
unicode(37) "*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 26 --
unicode(36) "+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 27 --
unicode(35) "-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 28 --
unicode(34) "./:;<=>?@hello123456he \x234 \101 "

-- Iteration 29 --
unicode(34) "./:;<=>?@hello123456he \x234 \101 "

-- Iteration 30 --
unicode(32) ":;<=>?@hello123456he \x234 \101 "

-- Iteration 31 --
unicode(31) ";<=>?@hello123456he \x234 \101 "

-- Iteration 32 --
unicode(30) "<=>?@hello123456he \x234 \101 "

-- Iteration 33 --
unicode(28) ">?@hello123456he \x234 \101 "

-- Iteration 34 --
unicode(29) "=>?@hello123456he \x234 \101 "

-- Iteration 35 --
unicode(27) "?@hello123456he \x234 \101 "

-- Iteration 36 --
unicode(26) "@hello123456he \x234 \101 "

-- Iteration 37 --
unicode(26) "@hello123456he \x234 \101 "

-- Iteration 38 --
unicode(2) "1 "

-- Iteration 39 --
unicode(5) "\101 "

-- Iteration 40 --
unicode(44) "#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 41 --
unicode(5) "\101 "

-- Iteration 42 --
bool(false)

-- Iteration 43 --
unicode(7) "4 \101 "

-- Iteration 44 --
unicode(37) "*+-./:;<=>?@hello123456he \x234 \101 "

-- Iteration 45 --
unicode(63) "Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 "
*** Done ***
