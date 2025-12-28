--TEST--
Test print_r() function with 64 bit integers
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

function check_printr( $variables ) {
    $counter = 1;
    foreach( $variables as $variable ) {
        echo "-- Iteration $counter --\n";
        print_r($variable, false);
        $ret_string = print_r($variable, true); //$ret_string captures the output
        echo "\n$ret_string\n";
        $counter++;
    }
}

$integers = [
  0x7FFFFFFF,  // max positive hexadecimal integer
  -0x80000000,  // min range of hexadecimal integer
  017777777777,  // max positive octal integer
  -020000000000  // min range of octal integer
];

check_printr($integers);

?>
--EXPECT--
-- Iteration 1 --
2147483647
2147483647
-- Iteration 2 --
-2147483648
-2147483648
-- Iteration 3 --
2147483647
2147483647
-- Iteration 4 --
-2147483648
-2147483648
