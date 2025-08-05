--TEST--
PFA errors: Some internal function parameters have UNKNOWN default value
--FILE--
<?php

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
ArgumentCountError: array_keys(): Argument #2 ($filter_value) must be passed explicitly, because the default value is not known
ArgumentCountError: array_keys(): Argument #2 ($filter_value) must be passed explicitly, because the default value is not known
