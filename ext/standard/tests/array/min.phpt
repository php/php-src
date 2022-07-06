--TEST--
min() tests
--INI--
precision=14
--FILE--
<?php

try {
    var_dump(min(1));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(min(array()));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(min(new stdclass));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(min(2,1,2));
var_dump(min(2.1,2.11,2.09));
var_dump(min("", "t", "b"));
var_dump(min(false, true, false));
var_dump(min(true, false, true));
var_dump(min(1, true, false, true));
var_dump(min(0, true, false, true));

?>
--EXPECT--
min(): Argument #1 ($value) must be of type array, int given
min(): Argument #1 ($value) must contain at least one element
min(): Argument #1 ($value) must be of type array, stdClass given
int(1)
float(2.09)
string(0) ""
bool(false)
bool(false)
bool(false)
int(0)
