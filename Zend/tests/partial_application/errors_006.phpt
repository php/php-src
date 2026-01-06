--TEST--
PFA errors: Some internal function parameters have UNKNOWN default value
--FILE--
<?php

$f = array_keys(?, 42);
var_dump($f([41, 42, 43]));

$f = array_keys(?, '42', true);
var_dump($f([41, 42, 43]));

$f = array_keys(?, '42', strict: ?);
var_dump($f([41, 42, 43], false));
var_dump($f([41, 42, 43], true));

try {
    // fn (array $array) => array_keys($array, ???, true)
    $f = array_keys(?, strict: true);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

try {
    // fn (array $array, mixed $filter_value = ???) => array_keys($array, $filter_value, true)
    $f = array_keys(?, strict: true, ...);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
array(0) {
}
ArgumentCountError: array_keys(): Argument #2 ($filter_value) must be passed explicitly, because the default value is not known
ArgumentCountError: array_keys(): Argument #2 ($filter_value) must be passed explicitly, because the default value is not known
