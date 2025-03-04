--TEST--
Test print_r() function with arrays
--INI--
precision=14
--FILE--
<?php

$values = [
  [],
  [true],
  [[], []],
  [[1, 2], ['a', 'b']],
  [1 => 'One'],
  ["test" => "is_array"],
  [0],
  [-1],
  [10.5, 5.6],
  ['string', 'test'],
];

foreach ($values as $value) {
    print_r($value, false);
    // $ret_string captures the output
    $ret_string = print_r($value, true);
    echo "\n$ret_string\n";
}

?>
--EXPECT--
Array
(
)

Array
(
)

Array
(
    [0] => 1
)

Array
(
    [0] => 1
)

Array
(
    [0] => Array
        (
        )

    [1] => Array
        (
        )

)

Array
(
    [0] => Array
        (
        )

    [1] => Array
        (
        )

)

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

Array
(
    [1] => One
)

Array
(
    [1] => One
)

Array
(
    [test] => is_array
)

Array
(
    [test] => is_array
)

Array
(
    [0] => 0
)

Array
(
    [0] => 0
)

Array
(
    [0] => -1
)

Array
(
    [0] => -1
)

Array
(
    [0] => 10.5
    [1] => 5.6
)

Array
(
    [0] => 10.5
    [1] => 5.6
)

Array
(
    [0] => string
    [1] => test
)

Array
(
    [0] => string
    [1] => test
)
