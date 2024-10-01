--TEST--
print_r(): Test printing of arrays
--INI--
precision=14
--WHITESPACE_SENSITIVE--
--FILE--
<?php

function check_print_r($variables) {
    $counter = 1;
    foreach($variables as $variable ) {
        echo "-- Iteration $counter --\n";
        ob_start();
        $should_be_true = print_r($variable, /* $return */ false);
        $output_content = ob_get_flush();
        var_dump($should_be_true);
        $print_r_with_return = print_r($variable, /* $return */ true);
        var_dump($output_content === $print_r_with_return);
        $output_content = null;
        $counter++;
    }
}

$arrays = [
    [],
    [null],
    [false],
    [true],
    [''],
    [ [], [] ],
    [ [1, 2], ['a', 'b'] ],
    [1 => 'One'],
    ["test" => "is_array"],
    [0],
    [-1],
    [10.5, 5.6],
    ["string", "test"],
    ['string', 'test'],
];

check_print_r($arrays);

?>
--EXPECT--
-- Iteration 1 --
Array
(
)
bool(true)
bool(true)
-- Iteration 2 --
Array
(
    [0] => 
)
bool(true)
bool(true)
-- Iteration 3 --
Array
(
    [0] => 
)
bool(true)
bool(true)
-- Iteration 4 --
Array
(
    [0] => 1
)
bool(true)
bool(true)
-- Iteration 5 --
Array
(
    [0] => 
)
bool(true)
bool(true)
-- Iteration 6 --
Array
(
    [0] => Array
        (
        )

    [1] => Array
        (
        )

)
bool(true)
bool(true)
-- Iteration 7 --
Array
(
    [0] => Array
        (
            [0] => 1
            [1] => 2
        )

    [1] => Array
        (
            [0] => a
            [1] => b
        )

)
bool(true)
bool(true)
-- Iteration 8 --
Array
(
    [1] => One
)
bool(true)
bool(true)
-- Iteration 9 --
Array
(
    [test] => is_array
)
bool(true)
bool(true)
-- Iteration 10 --
Array
(
    [0] => 0
)
bool(true)
bool(true)
-- Iteration 11 --
Array
(
    [0] => -1
)
bool(true)
bool(true)
-- Iteration 12 --
Array
(
    [0] => 10.5
    [1] => 5.6
)
bool(true)
bool(true)
-- Iteration 13 --
Array
(
    [0] => string
    [1] => test
)
bool(true)
bool(true)
-- Iteration 14 --
Array
(
    [0] => string
    [1] => test
)
bool(true)
bool(true)
