--TEST--
Test str_split() function : usage variations - different integer values for 'split_length' with heredoc 'str'
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
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
* passing different integer values for 'split_length' and heredoc string as 'str' argument to str_split()
*/

echo "*** Testing str_split() : different intger values for 'split_length' with heredoc 'str' ***\n";
//Initialise variables
$str = <<<EOT
string with 123,escape char \t.
EOT;

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
*** Testing str_split() : different intger values for 'split_length' with heredoc 'str' ***
-- Iteration 1 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
-- Iteration 2 --
array(30) {
  [0]=>
  unicode(1) "s"
  [1]=>
  unicode(1) "t"
  [2]=>
  unicode(1) "r"
  [3]=>
  unicode(1) "i"
  [4]=>
  unicode(1) "n"
  [5]=>
  unicode(1) "g"
  [6]=>
  unicode(1) " "
  [7]=>
  unicode(1) "w"
  [8]=>
  unicode(1) "i"
  [9]=>
  unicode(1) "t"
  [10]=>
  unicode(1) "h"
  [11]=>
  unicode(1) " "
  [12]=>
  unicode(1) "1"
  [13]=>
  unicode(1) "2"
  [14]=>
  unicode(1) "3"
  [15]=>
  unicode(1) ","
  [16]=>
  unicode(1) "e"
  [17]=>
  unicode(1) "s"
  [18]=>
  unicode(1) "c"
  [19]=>
  unicode(1) "a"
  [20]=>
  unicode(1) "p"
  [21]=>
  unicode(1) "e"
  [22]=>
  unicode(1) " "
  [23]=>
  unicode(1) "c"
  [24]=>
  unicode(1) "h"
  [25]=>
  unicode(1) "a"
  [26]=>
  unicode(1) "r"
  [27]=>
  unicode(1) " "
  [28]=>
  unicode(1) "	"
  [29]=>
  unicode(1) "."
}
-- Iteration 3 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
-- Iteration 4 --
array(1) {
  [0]=>
  unicode(30) "string with 123,escape char 	."
}
-- Iteration 5 --
array(2) {
  [0]=>
  unicode(26) "string with 123,escape cha"
  [1]=>
  unicode(4) "r 	."
}
-- Iteration 6 --
array(1) {
  [0]=>
  unicode(30) "string with 123,escape char 	."
}
-- Iteration 7 --
array(1) {
  [0]=>
  unicode(30) "string with 123,escape char 	."
}
-- Iteration 8 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
Done
