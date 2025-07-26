--TEST--
Test stripos() function : usage variations - double quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
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

  //nulls
  "\0",

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
--EXPECT--
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
int(8)
bool(false)
-- Iteration 11 --
int(0)
int(10)
-- Iteration 12 --
int(10)
int(11)
-- Iteration 13 --
int(12)
int(12)
-- Iteration 14 --
int(11)
bool(false)
-- Iteration 15 --
int(13)
bool(false)
-- Iteration 16 --
int(14)
bool(false)
-- Iteration 17 --
int(16)
int(16)
-- Iteration 18 --
int(17)
int(17)
-- Iteration 19 --
int(20)
int(20)
-- Iteration 20 --
int(22)
int(22)
-- Iteration 21 --
int(23)
int(23)
-- Iteration 22 --
int(24)
int(24)
-- Iteration 23 --
int(25)
int(25)
-- Iteration 24 --
bool(false)
bool(false)
-- Iteration 25 --
int(27)
int(27)
-- Iteration 26 --
int(28)
int(28)
-- Iteration 27 --
int(29)
int(29)
-- Iteration 28 --
int(31)
int(31)
-- Iteration 29 --
int(30)
int(30)
-- Iteration 30 --
int(32)
int(32)
-- Iteration 31 --
int(33)
int(33)
-- Iteration 32 --
int(33)
int(33)
-- Iteration 33 --
int(39)
int(39)
-- Iteration 34 --
int(15)
int(48)
-- Iteration 35 --
int(15)
int(48)
-- Iteration 36 --
int(51)
int(51)
-- Iteration 37 --
int(51)
int(51)
-- Iteration 38 --
bool(false)
bool(false)
-- Iteration 39 --
int(0)
bool(false)
*** Done ***
