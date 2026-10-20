--TEST--
Test var_dump() function with 64 bit integers
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
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
  0x7FFFFFFF,  // max positive hexadecimal integer
  -0x80000000,  // min range of hexadecimal integer
  017777777777,  // max positive octal integer
  -020000000000,  // min range of octal integer
  +2147483648, // max 32bit positive integer + 1
  -2147483648, // min 32bit positive integer - 1
];

check_var_dump($integers);

?>
--EXPECT--
-- Iteration 1 --
int(2147483647)
-- Iteration 2 --
int(-2147483648)
-- Iteration 3 --
int(2147483647)
-- Iteration 4 --
int(-2147483648)
-- Iteration 5 --
int(2147483648)
-- Iteration 6 --
int(-2147483648)
