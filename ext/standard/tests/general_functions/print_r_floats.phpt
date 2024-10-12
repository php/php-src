--TEST--
print_r(): Test printing floats
--FILE--
<?php

function check_print_r($variables) {
    $counter = 1;
    foreach($variables as $variable ) {
        echo "-- Iteration $counter --\n";
        ob_start();
        $should_be_true = print_r($variable, /* $return */ false);
        $output_content = ob_get_flush();
        echo PHP_EOL;
        var_dump($should_be_true);
        $print_r_with_return = print_r($variable, /* $return */ true);
        var_dump($output_content === $print_r_with_return);
        $output_content = null;
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
    PHP_INT_MIN*2,
    PHP_INT_MAX*3,
];

check_print_r($floats);

?>
--EXPECT--
-- Iteration 1 --
-0
bool(true)
bool(true)
-- Iteration 2 --
0
bool(true)
bool(true)
-- Iteration 3 --
1.234
bool(true)
bool(true)
-- Iteration 4 --
-1.234
bool(true)
bool(true)
-- Iteration 5 --
-2
bool(true)
bool(true)
-- Iteration 6 --
2
bool(true)
bool(true)
-- Iteration 7 --
-0.5
bool(true)
bool(true)
-- Iteration 8 --
0.567
bool(true)
bool(true)
-- Iteration 9 --
-0.00067
bool(true)
bool(true)
-- Iteration 10 --
670
bool(true)
bool(true)
-- Iteration 11 --
-0.00410003
bool(true)
bool(true)
-- Iteration 12 --
0.004100003
bool(true)
bool(true)
-- Iteration 13 --
100000
bool(true)
bool(true)
-- Iteration 14 --
-100000
bool(true)
bool(true)
-- Iteration 15 --
1.0E-5
bool(true)
bool(true)
-- Iteration 16 --
-1.0E-5
bool(true)
bool(true)
-- Iteration 17 --
100000
bool(true)
bool(true)
-- Iteration 18 --
-100000
bool(true)
bool(true)
-- Iteration 19 --
-1.844674407371E+19
bool(true)
bool(true)
-- Iteration 20 --
2.7670116110564E+19
bool(true)
bool(true)
