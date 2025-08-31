--TEST--
Test strripos() function : usage variations - single quoted strings for 'haystack' & 'needle' arguments
--FILE--
<?php
/* Test strripos() function by passing single quoted strings to 'haystack' & 'needle' arguments */

echo "*** Testing strripos() function: with single quoted strings ***\n";
$haystack = 'Hello,\t\n\0\n  $&!#%()*<=>?@hello123456he \x234 \101 ';
$needles = array(
          //regular strings
/*1*/	  'l',
          'L',
          'HELLO',
          'hEllo',

          //escape characters
/*5*/	  '\t',
          '\T',
          '     ',
          '\n',
          '\N',
          '
        ',  //new line

          //nulls
/*11*/	  '\0',

          //boolean false
/*14*/	  FALSE,
          false,

          //empty string
/*16*/	  '',

          //special chars
/*17*/	  ' ',
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

/*29*/	  '12345', 	//decimal numeric string
          '\x23',	//hexadecimal numeric string
          '#',  	//respective ASCII char of \x23
          '\101',  	//octal numeric string
          'A', 		// respective ASCII char for \101
          '456HEE', //numerics + chars
          42, 		//needle as int(ASCII value of '*')
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
*** Testing strripos() function: with single quoted strings ***
-- Iteration 1 --
int(32)
int(32)
int(32)
int(32)
-- Iteration 2 --
int(32)
int(32)
int(32)
int(32)
-- Iteration 3 --
int(29)
int(29)
int(29)
int(29)
-- Iteration 4 --
int(29)
int(29)
int(29)
int(29)
-- Iteration 5 --
int(6)
int(6)
bool(false)
int(6)
-- Iteration 6 --
int(6)
int(6)
bool(false)
int(6)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 8 --
int(12)
int(12)
bool(false)
int(12)
-- Iteration 9 --
int(12)
int(12)
bool(false)
int(12)
-- Iteration 10 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 11 --
int(10)
int(10)
bool(false)
int(10)
-- Iteration 12 --
int(54)
int(54)
int(54)
int(53)
-- Iteration 13 --
int(54)
int(54)
int(54)
int(53)
-- Iteration 14 --
int(54)
int(54)
int(54)
int(53)
-- Iteration 15 --
int(53)
int(53)
int(53)
int(53)
-- Iteration 16 --
int(16)
int(16)
bool(false)
int(16)
-- Iteration 17 --
int(15)
int(15)
bool(false)
int(15)
-- Iteration 18 --
int(17)
int(17)
bool(false)
int(17)
-- Iteration 19 --
int(18)
int(18)
bool(false)
int(18)
-- Iteration 20 --
int(21)
int(21)
int(21)
int(21)
-- Iteration 21 --
int(24)
int(24)
int(24)
int(24)
-- Iteration 22 --
int(26)
int(26)
int(26)
int(26)
-- Iteration 23 --
int(25)
int(25)
int(25)
int(25)
-- Iteration 24 --
int(27)
int(27)
int(27)
int(27)
-- Iteration 25 --
int(28)
int(28)
int(28)
int(28)
-- Iteration 26 --
int(28)
int(28)
int(28)
int(28)
-- Iteration 27 --
int(34)
int(34)
int(34)
int(34)
-- Iteration 28 --
int(43)
int(43)
int(43)
int(43)
-- Iteration 29 --
int(19)
int(19)
bool(false)
int(19)
-- Iteration 30 --
int(49)
int(49)
int(49)
int(49)
-- Iteration 31 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 32 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 33 --
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 34 --
int(0)
bool(false)
bool(false)
int(0)
