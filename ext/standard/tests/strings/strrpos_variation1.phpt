--TEST--
Test strrpos() function : usage variations - double quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
/* Test strrpos() function by passing double quoted strings for 'haystack' & 'needle' arguments */

echo "*** Testing strrpos() function: with double quoted strings ***\n";
$haystack = "Hello,\t\n\0\n  $&!#%()*<=>?@hello123456he \x234 \101 ";
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
  "()",
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
  var_dump( strrpos($haystack, $needle[$index]) );
  var_dump( strrpos($haystack, $needle[$index], $index) );
  $count++;
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function: with double quoted strings ***
-- Iteration 1 --
int(28)
int(28)
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
int(9)
int(9)
-- Iteration 9 --
bool(false)
bool(false)
-- Iteration 10 --
int(9)
int(9)
-- Iteration 11 --
int(8)
bool(false)
-- Iteration 12 --
int(44)
int(44)
-- Iteration 13 --
int(44)
int(44)
-- Iteration 14 --
int(44)
int(44)
-- Iteration 15 --
int(44)
int(44)
-- Iteration 16 --
int(44)
int(44)
-- Iteration 17 --
int(43)
int(43)
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
int(17)
bool(false)
-- Iteration 23 --
int(20)
bool(false)
-- Iteration 24 --
int(22)
bool(false)
-- Iteration 25 --
int(21)
bool(false)
-- Iteration 26 --
int(23)
bool(false)
-- Iteration 27 --
int(24)
bool(false)
-- Iteration 28 --
bool(false)
bool(false)
-- Iteration 29 --
int(30)
int(30)
-- Iteration 30 --
int(39)
int(39)
-- Iteration 31 --
int(39)
int(39)
-- Iteration 32 --
int(42)
int(42)
-- Iteration 33 --
int(42)
int(42)
-- Iteration 34 --
bool(false)
bool(false)
-- Iteration 35 --
int(0)
bool(false)
*** Done ***
