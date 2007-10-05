--TEST--
Test str_split() function : basic functionality 
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
--EXPECTF--
*** Testing str_split() : basic functionality ***
-- With all possible arguments --
array(5) {
  [0]=>
  string(5) "This "
  [1]=>
  string(5) "is ba"
  [2]=>
  string(5) "sic t"
  [3]=>
  string(5) "estca"
  [4]=>
  string(2) "se"
}
-- With split_length as default argument --
array(22) {
  [0]=>
  string(1) "T"
  [1]=>
  string(1) "h"
  [2]=>
  string(1) "i"
  [3]=>
  string(1) "s"
  [4]=>
  string(1) " "
  [5]=>
  string(1) "i"
  [6]=>
  string(1) "s"
  [7]=>
  string(1) " "
  [8]=>
  string(1) "b"
  [9]=>
  string(1) "a"
  [10]=>
  string(1) "s"
  [11]=>
  string(1) "i"
  [12]=>
  string(1) "c"
  [13]=>
  string(1) " "
  [14]=>
  string(1) "t"
  [15]=>
  string(1) "e"
  [16]=>
  string(1) "s"
  [17]=>
  string(1) "t"
  [18]=>
  string(1) "c"
  [19]=>
  string(1) "a"
  [20]=>
  string(1) "s"
  [21]=>
  string(1) "e"
}
Done
