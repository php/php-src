--TEST--
filter_input_array: test FILTER_NULL_ON_FAILURE option does not affect general result on empty input
--EXTENSIONS--
filter
--FILE--
<?php
$args = [
    "c" => [
        "flags" => FILTER_NULL_ON_FAILURE,
    ]
];

var_dump(filter_input_array(INPUT_GET, $args, true));
?>
--EXPECT--
NULL
