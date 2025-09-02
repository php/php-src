--TEST--
Test var_dump() function with integers
--INI--
precision=14
--FILE--
<?php

function check_var_dump( $variables ) {
    $counter = 1;
    foreach( $variables as $variable ) {
        echo "-- Iteration $counter --\n";
        var_dump($variable);
        $counter++;
    }
}

$integers = [
  0,  // zero as argument
  000000123,  //octal value of 83
  123000000,
  -00000123,  //octal value of 83
  -12300000,
  range(1,10),  // positive values
  range(-1,-10),  // negative values
  +2147483647,  // max 32bit integer
  -2147483647,  // min 32bit integer
];

check_var_dump($integers);

?>
--EXPECT--
-- Iteration 1 --
int(0)
-- Iteration 2 --
int(83)
-- Iteration 3 --
int(123000000)
-- Iteration 4 --
int(-83)
-- Iteration 5 --
int(-12300000)
-- Iteration 6 --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
-- Iteration 7 --
array(10) {
  [0]=>
  int(-1)
  [1]=>
  int(-2)
  [2]=>
  int(-3)
  [3]=>
  int(-4)
  [4]=>
  int(-5)
  [5]=>
  int(-6)
  [6]=>
  int(-7)
  [7]=>
  int(-8)
  [8]=>
  int(-9)
  [9]=>
  int(-10)
}
-- Iteration 8 --
int(2147483647)
-- Iteration 9 --
int(-2147483647)
