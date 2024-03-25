--TEST--
filter_input_array: test FILTER_NULL_ON_FAILURE option
--EXTENSIONS--
filter
--FILE--
<?php
$args = [
    "c" => [
        "flags" => FILTER_NULL_ON_FAILURE,
    ]
];

var_dump(filter_input_array(INPUT_GET, $args, true, true));
var_dump(filter_input_array(INPUT_GET, $args, true));
var_dump(filter_input_array(INPUT_GET, FILTER_DEFAULT, true, true));
var_dump(filter_input_array(INPUT_GET, FILTER_DEFAULT, true));
?>
--EXPECT--
bool(false)
NULL
bool(false)
NULL