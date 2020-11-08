--TEST--
max() tests
--INI--
precision=14
--FILE--
<?php

try {
    var_dump(max(1));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(max(array()));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(max(new stdclass));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(max(2,1,2));
var_dump(max(2.1,2.11,2.09));
var_dump(max("", "t", "b"));
var_dump(max(false, true, false));
var_dump(max(true, false, true));
var_dump(max(1, true, false, true));
var_dump(max(0, true, false, true));

?>
--EXPECT--
max(): Argument #1 ($value) must be of type array, int given
max(): Argument #1 ($value) must contain at least one element
max(): Argument #1 ($value) must be of type array, stdClass given
int(2)
float(2.11)
string(1) "t"
bool(true)
bool(true)
int(1)
bool(true)
