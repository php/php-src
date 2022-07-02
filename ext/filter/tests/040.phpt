--TEST--
filter_has_var() tests
--EXTENSIONS--
filter
--GET--
a=1&b=2&c=0
--POST--
ap[]=1&bp=test&cp=
--FILE--
<?php

var_dump(filter_has_var(INPUT_GET,""));
var_dump(filter_has_var(INPUT_POST, "ap"));
var_dump(filter_has_var(INPUT_POST, "cp"));
var_dump(filter_has_var(INPUT_GET, "a"));
var_dump(filter_has_var(INPUT_GET, "c"));
var_dump(filter_has_var(INPUT_GET, "abc"));
var_dump(filter_has_var(INPUT_GET, "cc"));
try {
    filter_has_var(-1, "cc");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(filter_has_var(0, "cc"));

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
filter_has_var(): Argument #1 ($input_type) must be an INPUT_* constant
bool(false)
Done
