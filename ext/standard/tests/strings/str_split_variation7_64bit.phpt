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
  string(1) "s"
  [1]=>
  string(1) "t"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "i"
  [4]=>
  string(1) "n"
  [5]=>
  string(1) "g"
  [6]=>
  string(1) " "
  [7]=>
  string(1) "w"
  [8]=>
  string(1) "i"
  [9]=>
  string(1) "t"
  [10]=>
  string(1) "h"
  [11]=>
  string(1) " "
  [12]=>
  string(1) "1"
  [13]=>
  string(1) "2"
  [14]=>
  string(1) "3"
  [15]=>
  string(1) ","
  [16]=>
  string(1) "e"
  [17]=>
  string(1) "s"
  [18]=>
  string(1) "c"
  [19]=>
  string(1) "a"
  [20]=>
  string(1) "p"
  [21]=>
  string(1) "e"
  [22]=>
  string(1) " "
  [23]=>
  string(1) "c"
  [24]=>
  string(1) "h"
  [25]=>
  string(1) "a"
  [26]=>
  string(1) "r"
  [27]=>
  string(1) " "
  [28]=>
  string(1) "	"
  [29]=>
  string(1) "."
}
-- Iteration 3 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
-- Iteration 4 --
array(1) {
  [0]=>
  string(30) "string with 123,escape char 	."
}
-- Iteration 5 --
array(2) {
  [0]=>
  string(26) "string with 123,escape cha"
  [1]=>
  string(4) "r 	."
}
-- Iteration 6 --
array(1) {
  [0]=>
  string(30) "string with 123,escape char 	."
}
-- Iteration 7 --
array(1) {
  [0]=>
  string(30) "string with 123,escape char 	."
}
-- Iteration 8 --

Warning: str_split(): The length of each segment must be greater than zero in %s on line %d
bool(false)
Done
