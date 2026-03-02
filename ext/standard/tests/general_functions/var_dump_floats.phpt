--TEST--
Test var_dump() function with floats
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

$floats = [
  -0.0,
  +0.0,
  1.234,
  -1.234,
  -2.000000,
  000002.00,
  -.5,
  .567,
  -.6700000e-3,
  .6700000e+3,
  -4.10003e-3,
  4.100003e-3,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
];
check_var_dump($floats);

?>
--EXPECT--
-- Iteration 1 --
float(-0)
-- Iteration 2 --
float(0)
-- Iteration 3 --
float(1.234)
-- Iteration 4 --
float(-1.234)
-- Iteration 5 --
float(-2)
-- Iteration 6 --
float(2)
-- Iteration 7 --
float(-0.5)
-- Iteration 8 --
float(0.567)
-- Iteration 9 --
float(-0.00067)
-- Iteration 10 --
float(670)
-- Iteration 11 --
float(-0.00410003)
-- Iteration 12 --
float(0.004100003)
-- Iteration 13 --
float(100000)
-- Iteration 14 --
float(-100000)
-- Iteration 15 --
float(1.0E-5)
-- Iteration 16 --
float(-1.0E-5)
-- Iteration 17 --
float(100000)
-- Iteration 18 --
float(-100000)
