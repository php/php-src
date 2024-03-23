--TEST--
filter_input_array: test FILTER_NULL_ON_FAILURE option
--EXTENSIONS--
filter
--GET--
b="test"
--FILE--
<?php
    $args = [
        "a" => [
            "flags" => FILTER_NULL_ON_FAILURE,
            "filter" => FILTER_VALIDATE_BOOLEAN,
        ],
        "b" => [
            "filter" => FILTER_VALIDATE_BOOLEAN,
        ]
    ];

    var_dump(filter_input_array(INPUT_GET, $args, true));
?>
--EXPECT--
array(2) {
  ["a"]=>
  NULL
  ["b"]=>
  bool(false)
}
