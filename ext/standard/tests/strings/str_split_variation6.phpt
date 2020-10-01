--TEST--
Test str_split() function : usage variations - different integer values for 'split_length' argument
--FILE--
<?php
/*
* passing different integer values for 'split_length' argument to str_split()
*/

echo "*** Testing str_split() : different integer values for 'split_length' ***\n";
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
  -2147483648,  //min negative integer
);

//loop through each element of $values for 'split_length'
for($count = 0; $count < count($values); $count++) {
    echo "-- Iteration ".($count + 1)." --\n";

    try {
        var_dump( str_split($str, $values[$count]) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . "\n";
    }
}
?>
--EXPECT--
*** Testing str_split() : different integer values for 'split_length' ***
-- Iteration 1 --
str_split(): Argument #2 ($length) must be greater than 0
-- Iteration 2 --
array(42) {
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
  string(1) "a"
  [9]=>
  string(1) " "
  [10]=>
  string(1) "s"
  [11]=>
  string(1) "t"
  [12]=>
  string(1) "r"
  [13]=>
  string(1) "i"
  [14]=>
  string(1) "n"
  [15]=>
  string(1) "g"
  [16]=>
  string(1) " "
  [17]=>
  string(1) "w"
  [18]=>
  string(1) "i"
  [19]=>
  string(1) "t"
  [20]=>
  string(1) "h"
  [21]=>
  string(1) " "
  [22]=>
  string(1) "1"
  [23]=>
  string(1) "2"
  [24]=>
  string(1) "3"
  [25]=>
  string(1) " "
  [26]=>
  string(1) "&"
  [27]=>
  string(1) " "
  [28]=>
  string(1) "e"
  [29]=>
  string(1) "s"
  [30]=>
  string(1) "c"
  [31]=>
  string(1) "a"
  [32]=>
  string(1) "p"
  [33]=>
  string(1) "e"
  [34]=>
  string(1) " "
  [35]=>
  string(1) "c"
  [36]=>
  string(1) "h"
  [37]=>
  string(1) "a"
  [38]=>
  string(1) "r"
  [39]=>
  string(1) " "
  [40]=>
  string(1) "\"
  [41]=>
  string(1) "t"
}
-- Iteration 3 --
str_split(): Argument #2 ($length) must be greater than 0
-- Iteration 4 --
array(1) {
  [0]=>
  string(42) "This is a string with 123 & escape char \t"
}
-- Iteration 5 --
array(2) {
  [0]=>
  string(26) "This is a string with 123 "
  [1]=>
  string(16) "& escape char \t"
}
-- Iteration 6 --
array(1) {
  [0]=>
  string(42) "This is a string with 123 & escape char \t"
}
-- Iteration 7 --
str_split(): Argument #2 ($length) must be greater than 0
