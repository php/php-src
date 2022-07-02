--TEST--
Test strripos() function : usage variations - double quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
/* Test strripos() function by passing double quoted strings for 'haystack' & 'needle' arguments */

echo "*** Testing strripos() function: with double quoted strings ***\n";
$haystack = "Hello,\t\n\0\n  $&!#%()*<=>?@hello123456he \x234 \101 ";
$needles = array(
          //regular strings
/*1*/	  "l",
          "L",
          "HELLO",
          "hEllo",

          //escape characters
/*5*/	  "\t",
          "\T",  //invalid input
          "     ",
          "\n",
          "\N",  //invalid input
          "
",  //new line

          //nulls
/*11*/	  "\0",

          //boolean false
/*14*/	  FALSE,
          false,

          //empty string
/*16*/	  "",

          //special chars
/*17*/	  " ",
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

/*29*/	  "12345", //decimal numeric string
          "\x23",  //hexadecimal numeric string
          "#",  //respective ASCII char of \x23
          "\101",  //octal numeric string
          "A",  //respective ASCII char of \101
          "456HEE",  //numerics + chars
          $haystack  //haystack as needle
);

/* loop through to get the position of the needle in haystack string */
$count = 1;
foreach ($needles as $needle) {
  echo "-- Iteration $count --\n";
  var_dump( strripos($haystack, $needle) );
  var_dump( strripos($haystack, $needle, 1) );
  var_dump( strripos($haystack, $needle, 20) );
  var_dump( strripos($haystack, $needle, -1) );
  $count++;
}
?>
--EXPECT--
*** Testing strripos() function: with double quoted strings ***
-- Iteration 1 --
int(28)
int(28)
int(28)
int(28)
-- Iteration 2 --
int(28)
int(28)
int(28)
int(28)
-- Iteration 3 --
int(25)
int(25)
int(25)
int(25)
-- Iteration 4 --
int(25)
int(25)
int(25)
int(25)
-- Iteration 5 --
int(6)
int(6)
bool(false)
int(6)
-- Iteration 6 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 8 --
int(9)
int(9)
bool(false)
int(9)
-- Iteration 9 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 10 --
int(9)
int(9)
bool(false)
int(9)
-- Iteration 11 --
int(8)
int(8)
bool(false)
int(8)
-- Iteration 12 --
int(44)
int(44)
int(44)
int(43)
-- Iteration 13 --
int(44)
int(44)
int(44)
int(43)
-- Iteration 14 --
int(44)
int(44)
int(44)
int(43)
-- Iteration 15 --
int(43)
int(43)
int(43)
int(43)
-- Iteration 16 --
int(12)
int(12)
bool(false)
int(12)
-- Iteration 17 --
int(11)
int(11)
bool(false)
int(11)
-- Iteration 18 --
int(13)
int(13)
bool(false)
int(13)
-- Iteration 19 --
int(14)
int(14)
bool(false)
int(14)
-- Iteration 20 --
int(17)
int(17)
bool(false)
int(17)
-- Iteration 21 --
int(20)
int(20)
int(20)
int(20)
-- Iteration 22 --
int(22)
int(22)
int(22)
int(22)
-- Iteration 23 --
int(21)
int(21)
int(21)
int(21)
-- Iteration 24 --
int(23)
int(23)
int(23)
int(23)
-- Iteration 25 --
int(24)
int(24)
int(24)
int(24)
-- Iteration 26 --
int(24)
int(24)
int(24)
int(24)
-- Iteration 27 --
int(30)
int(30)
int(30)
int(30)
-- Iteration 28 --
int(39)
int(39)
int(39)
int(39)
-- Iteration 29 --
int(39)
int(39)
int(39)
int(39)
-- Iteration 30 --
int(42)
int(42)
int(42)
int(42)
-- Iteration 31 --
int(42)
int(42)
int(42)
int(42)
-- Iteration 32 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 33 --
int(0)
bool(false)
bool(false)
int(0)
