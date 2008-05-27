--TEST--
Test str_split() function : basic functionality(Bug#42866)
--FILE--
<?php
/* Prototype  : array str_split(string $str [, int $split_length])
 * Description: Convert a string to an array. If split_length is 
                specified, break the string down into chunks each 
                split_length characters long. 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing str_split() : basic functionality ***\n";

// Initialise all required variables
$str = 'This is basic testcase';
$split_length = 5;

// Calling str_split() with all possible arguments
echo "-- With all possible arguments --\n";
var_dump( str_split($str,$split_length) );

// Calling str_split() with default arguments
echo "-- With split_length as default argument --\n";
var_dump( str_split($str) );

echo "Done"
?>
--EXPECT--
*** Testing str_split() : basic functionality ***
-- With all possible arguments --
array(5) {
  [0]=>
  unicode(5) "This "
  [1]=>
  unicode(5) "is ba"
  [2]=>
  unicode(5) "sic t"
  [3]=>
  unicode(5) "estca"
  [4]=>
  unicode(2) "se"
}
-- With split_length as default argument --
array(22) {
  [0]=>
  unicode(1) "T"
  [1]=>
  unicode(1) "h"
  [2]=>
  unicode(1) "i"
  [3]=>
  unicode(1) "s"
  [4]=>
  unicode(1) " "
  [5]=>
  unicode(1) "i"
  [6]=>
  unicode(1) "s"
  [7]=>
  unicode(1) " "
  [8]=>
  unicode(1) "b"
  [9]=>
  unicode(1) "a"
  [10]=>
  unicode(1) "s"
  [11]=>
  unicode(1) "i"
  [12]=>
  unicode(1) "c"
  [13]=>
  unicode(1) " "
  [14]=>
  unicode(1) "t"
  [15]=>
  unicode(1) "e"
  [16]=>
  unicode(1) "s"
  [17]=>
  unicode(1) "t"
  [18]=>
  unicode(1) "c"
  [19]=>
  unicode(1) "a"
  [20]=>
  unicode(1) "s"
  [21]=>
  unicode(1) "e"
}
Done
