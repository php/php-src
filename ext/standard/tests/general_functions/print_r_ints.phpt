--TEST--
Test print_r() function with integers
--INI--
precision=14
--FILE--
<?php

function check_printr( $variables ) {
    $counter = 1;
    foreach( $variables as $variable ) {
        echo "\n-- Iteration $counter --\n";
        print_r($variable, false);
        $ret_string = print_r($variable, true); //$ret_string captures the output
        echo "\n$ret_string\n";
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
  +2147483647,  // max positive integer
  +2147483648,  // max positive integer + 1
  -2147483648,  // min range of integer
  -2147483647,  // min range of integer + 1
];

check_printr($integers);

?>
--EXPECT--
-- Iteration 1 --
0
0

-- Iteration 2 --
83
83

-- Iteration 3 --
123000000
123000000

-- Iteration 4 --
-83
-83

-- Iteration 5 --
-12300000
-12300000

-- Iteration 6 --
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 10
)

Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 10
)


-- Iteration 7 --
Array
(
    [0] => -1
    [1] => -2
    [2] => -3
    [3] => -4
    [4] => -5
    [5] => -6
    [6] => -7
    [7] => -8
    [8] => -9
    [9] => -10
)

Array
(
    [0] => -1
    [1] => -2
    [2] => -3
    [3] => -4
    [4] => -5
    [5] => -6
    [6] => -7
    [7] => -8
    [8] => -9
    [9] => -10
)


-- Iteration 8 --
2147483647
2147483647

-- Iteration 9 --
2147483648
2147483648

-- Iteration 10 --
-2147483648
-2147483648

-- Iteration 11 --
-2147483647
-2147483647
