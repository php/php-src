--TEST--
Test print_r() function with arrays, output has trailing whitespaces
--WHITESPACE_SENSITIVE--
--FILE--
<?php

$values = [
  ['null' => null],
  ['false' => false],
  ['empty' => ''],
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
    [null] => 
)

Array
(
    [null] => 
)

Array
(
    [false] => 
)

Array
(
    [false] => 
)

Array
(
    [empty] => 
)

Array
(
    [empty] => 
)
