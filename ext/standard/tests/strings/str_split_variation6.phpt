--TEST--
Test str_split() function : usage variations - different integer values for 'split_length' argument(Bug#42866) 
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : array str_split(string $str [, int $split_length])
 * Description: Convert a string to an array. If split_length is 
                specified, break the string down into chunks each 
                split_length characters long. 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* passing different integer values for 'split_length' argument to str_split()
*/

echo "*** Testing str_split() : different intger values for 'split_length' ***\n";
//Initialise variables
$str = 'This is a string with 123 & escape char \t';

//different values for 'split_length' 
$values = array (
  0,
  1,
  -123,  //negative integer
  0234,  //octal number
  0x1A,  //hexadecimal number
  2147483647,  //max positive integer number
  2147483648,  //max positive integer+1
  -2147483648,  //min negative integer
);

//loop through each element of $values for 'split_length'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( str_split($str, $values[$count]) );
}
echo "Done"
?>
--EXPECTF--
*** Testing str_split() : different intger values for 'split_length' ***
-- Iteration 1 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
-- Iteration 2 --
array(42) {
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
  unicode(1) "a"
  [9]=>
  unicode(1) " "
  [10]=>
  unicode(1) "s"
  [11]=>
  unicode(1) "t"
  [12]=>
  unicode(1) "r"
  [13]=>
  unicode(1) "i"
  [14]=>
  unicode(1) "n"
  [15]=>
  unicode(1) "g"
  [16]=>
  unicode(1) " "
  [17]=>
  unicode(1) "w"
  [18]=>
  unicode(1) "i"
  [19]=>
  unicode(1) "t"
  [20]=>
  unicode(1) "h"
  [21]=>
  unicode(1) " "
  [22]=>
  unicode(1) "1"
  [23]=>
  unicode(1) "2"
  [24]=>
  unicode(1) "3"
  [25]=>
  unicode(1) " "
  [26]=>
  unicode(1) "&"
  [27]=>
  unicode(1) " "
  [28]=>
  unicode(1) "e"
  [29]=>
  unicode(1) "s"
  [30]=>
  unicode(1) "c"
  [31]=>
  unicode(1) "a"
  [32]=>
  unicode(1) "p"
  [33]=>
  unicode(1) "e"
  [34]=>
  unicode(1) " "
  [35]=>
  unicode(1) "c"
  [36]=>
  unicode(1) "h"
  [37]=>
  unicode(1) "a"
  [38]=>
  unicode(1) "r"
  [39]=>
  unicode(1) " "
  [40]=>
  unicode(1) "\"
  [41]=>
  unicode(1) "t"
}
-- Iteration 3 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
-- Iteration 4 --
array(1) {
  [0]=>
  unicode(42) "This is a string with 123 & escape char \t"
}
-- Iteration 5 --
array(2) {
  [0]=>
  unicode(26) "This is a string with 123 "
  [1]=>
  unicode(16) "& escape char \t"
}
-- Iteration 6 --
array(1) {
  [0]=>
  unicode(42) "This is a string with 123 & escape char \t"
}
-- Iteration 7 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
-- Iteration 8 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
Done
