--TEST--
Test var_dump() function with 32 bit integers
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
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
  +2147483648, // max 32bit positive integer + 1
  -2147483648, // min 32bit positive integer - 1
];

check_var_dump($integers);

?>
--EXPECT--
-- Iteration 1 --
float(2147483648)
-- Iteration 2 --
float(-2147483648)
