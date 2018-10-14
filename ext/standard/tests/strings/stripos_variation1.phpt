--TEST--
Test stripos() function : usage variations - double quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function by passing double quoted strings for 'haystack' & 'needle' arguments */

echo "*** Testing stripos() function: with double quoted strings ***\n";
$haystack = "Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 ";
$needle = array(
  //regular strings
  "l",
  "L",
  "HELLO",
  "hEllo",

  //escape characters
  "\t",
  "\T",  //invalid input
  "     ",
  "\n",
  "\N",  //invalid input
  "
",  //new line

  //nulls
  "\0",
  NULL,
  null,

  //boolean false
  FALSE,
  false,

  //empty string
  "",

  //special chars
  " ",
  "$",
  " $",
  "&",
  "!#",
  "%\o",
  "\o,",
  "()",
  "*+",
  "+",
  "-",
  ".",
  ".;",
  ":;",
  ";",
  "<=>",
  ">",
  "=>",
  "?",
  "@",
  "@hEllo",

  "12345", //decimal numeric string
  "\x23",  //hexadecimal numeric string
  "#",  //respective ASCII char of \x23
  "\101",  //octal numeric string
  "A",  //respective ASCII char of \101
  "456HEE",  //numerics + chars
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
--EXPECTF--
*** Testing stripos() function: with double quoted strings ***
-- Iteration 1 --
int(2)
int(2)
-- Iteration 2 --
int(2)
int(2)
-- Iteration 3 --
int(0)
int(34)
-- Iteration 4 --
int(0)
int(34)
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
int(7)
int(7)
-- Iteration 9 --
bool(false)
bool(false)
-- Iteration 10 --
int(7)
int(9)
-- Iteration 11 --
int(8)
bool(false)
-- Iteration 12 --
int(8)
bool(false)
-- Iteration 13 --
int(8)
bool(false)
-- Iteration 14 --
int(8)
bool(false)
-- Iteration 15 --
int(8)
bool(false)
-- Iteration 16 --
bool(false)
bool(false)
-- Iteration 17 --
int(10)
int(47)
-- Iteration 18 --
int(12)
bool(false)
-- Iteration 19 --
int(11)
bool(false)
-- Iteration 20 --
int(13)
bool(false)
-- Iteration 21 --
int(14)
bool(false)
-- Iteration 22 --
int(16)
bool(false)
-- Iteration 23 --
int(17)
bool(false)
-- Iteration 24 --
int(20)
bool(false)
-- Iteration 25 --
int(22)
bool(false)
-- Iteration 26 --
int(23)
bool(false)
-- Iteration 27 --
int(24)
bool(false)
-- Iteration 28 --
int(25)
bool(false)
-- Iteration 29 --
bool(false)
bool(false)
-- Iteration 30 --
int(27)
bool(false)
-- Iteration 31 --
int(28)
bool(false)
-- Iteration 32 --
int(29)
bool(false)
-- Iteration 33 --
int(31)
bool(false)
-- Iteration 34 --
int(30)
bool(false)
-- Iteration 35 --
int(32)
bool(false)
-- Iteration 36 --
int(33)
bool(false)
-- Iteration 37 --
int(33)
bool(false)
-- Iteration 38 --
int(39)
int(39)
-- Iteration 39 --
int(15)
int(48)
-- Iteration 40 --
int(15)
int(48)
-- Iteration 41 --
int(51)
int(51)
-- Iteration 42 --
int(51)
int(51)
-- Iteration 43 --
bool(false)
bool(false)
-- Iteration 44 --
int(0)
bool(false)
*** Done ***
