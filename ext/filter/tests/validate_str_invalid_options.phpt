--TEST--
FILTER_VALIDATE_STR: invalid min_len/max_len options
--FILE--
<?php

echo "--- min_len negative ---\n";
var_dump(filter_var("abc", FILTER_VALIDATE_STR, [
    "options" => ["min_len" => -1]
]));

echo "--- max_len negative ---\n";
var_dump(filter_var("abc", FILTER_VALIDATE_STR, [
    "options" => ["max_len" => -1]
]));

echo "--- min_len greater than max_len ---\n";
var_dump(filter_var("abc", FILTER_VALIDATE_STR, [
    "options" => [
        "min_len" => 10,
        "max_len" => 5
    ]
]));

?>
--EXPECTF--
--- min_len negative ---

Warning: filter_var(): min_len must be greater than or equal to 0 in %s on line %d
bool(false)
--- max_len negative ---

Warning: filter_var(): max_len must be greater than or equal to 0 in %s on line %d
bool(false)
--- min_len greater than max_len ---

Warning: filter_var(): min_len must be less than or equal to max_len in %s on line %d
bool(false)