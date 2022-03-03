--TEST--
Using isset() with arrays
--FILE--
<?php

$array = [
    0 => true,
    "a" => true,
];

var_dump(isset($array[0]));

var_dump(isset($array["a"]));

var_dump(isset($array[false]));

var_dump(isset($array[0.6]));

var_dump(isset($array[true]));

var_dump(isset($array[null]));

var_dump(isset($array[STDIN]));

try {
    isset($array[[]]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    isset($array[new stdClass()]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Deprecated: Implicit conversion from float 0.6 to int loses precision in %s on line %d
bool(true)
bool(false)
bool(false)

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
bool(false)
Illegal offset type in isset or empty
Illegal offset type in isset or empty
