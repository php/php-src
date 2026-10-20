--TEST--
Test print_r() function with floats
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
  -.6700000E+3,
  .6700000E+3,
  .6700000e+3,
  -4.10003e-3,
  -4.10003E+3,
  4.100003e-3,
  4.100003E+3,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
  1E5,
  -1E5,
  1E+5,
  -1E+5,
  1E-5,
  -1E-5,
  -0x80000001,  // float value, beyond max negative int
  0x80000001,  // float value, beyond max positive int
  020000000001,  // float value, beyond max positive int
  -020000000001  // float value, beyond max negative int
];
check_printr($floats);

?>
--EXPECT--
-- Iteration 1 --
-0
-0

-- Iteration 2 --
0
0

-- Iteration 3 --
1.234
1.234

-- Iteration 4 --
-1.234
-1.234

-- Iteration 5 --
-2
-2

-- Iteration 6 --
2
2

-- Iteration 7 --
-0.5
-0.5

-- Iteration 8 --
0.567
0.567

-- Iteration 9 --
-0.00067
-0.00067

-- Iteration 10 --
-670
-670

-- Iteration 11 --
670
670

-- Iteration 12 --
670
670

-- Iteration 13 --
-0.00410003
-0.00410003

-- Iteration 14 --
-4100.03
-4100.03

-- Iteration 15 --
0.004100003
0.004100003

-- Iteration 16 --
4100.003
4100.003

-- Iteration 17 --
100000
100000

-- Iteration 18 --
-100000
-100000

-- Iteration 19 --
1.0E-5
1.0E-5

-- Iteration 20 --
-1.0E-5
-1.0E-5

-- Iteration 21 --
100000
100000

-- Iteration 22 --
-100000
-100000

-- Iteration 23 --
100000
100000

-- Iteration 24 --
-100000
-100000

-- Iteration 25 --
100000
100000

-- Iteration 26 --
-100000
-100000

-- Iteration 27 --
1.0E-5
1.0E-5

-- Iteration 28 --
-1.0E-5
-1.0E-5

-- Iteration 29 --
-2147483649
-2147483649

-- Iteration 30 --
2147483649
2147483649

-- Iteration 31 --
2147483649
2147483649

-- Iteration 32 --
-2147483649
-2147483649
