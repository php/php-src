--TEST--
print_r(): Test printing basic primitive types
--INI--
precision=14
--FILE--
<?php

function check_print_r($variables) {
    $counter = 1;
    foreach($variables as $variable ) {
        echo "-- Iteration $counter --\n";
        $should_be_true = print_r($variable, /* $return */ false);
        echo PHP_EOL;
        var_dump($should_be_true);
        var_dump(print_r($variable, /* $return */ true));
        $counter++;
    }
}

$basic_types = [
    null,
    false,
    true,
    0,
    42,
    -245,
    0o14,
    0xFA5,
    0b1101,
];

check_print_r($basic_types);

?>
--EXPECT--
-- Iteration 1 --

bool(true)
string(0) ""
-- Iteration 2 --

bool(true)
string(0) ""
-- Iteration 3 --
1
bool(true)
string(1) "1"
-- Iteration 4 --
0
bool(true)
string(1) "0"
-- Iteration 5 --
42
bool(true)
string(2) "42"
-- Iteration 6 --
-245
bool(true)
string(4) "-245"
-- Iteration 7 --
12
bool(true)
string(2) "12"
-- Iteration 8 --
4005
bool(true)
string(4) "4005"
-- Iteration 9 --
13
bool(true)
string(2) "13"
