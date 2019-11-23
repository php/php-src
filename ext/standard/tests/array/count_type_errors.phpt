--TEST--
Test count() : Only arrays and countable objects can be counted
--FILE--
<?php
/**
 * Prototype  : int count(array|Countable|null $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */

$typeErrorValues = [
    true,
    false,
    "",
    "string",
    "0",
    "100",
    "0.0",
    "-23.45",
    (object) [],
    new stdClass,
];

foreach ($typeErrorValues as $value) {
    echo "Test for: ";
    var_dump($value);
    try {
        var_dump(count($value));
    } catch (\TypeError $e) {
        echo $e->getMessage() . "\n";
    }
    try {
        var_dump(count($value, COUNT_NORMAL));
    } catch (\TypeError $e) {
        echo $e->getMessage() . "\n";
    }
    try {
        var_dump(count($value, COUNT_RECURSIVE));
    } catch (\TypeError $e) {
        echo $e->getMessage() . "\n";
    }
}

--EXPECT--
Test for: bool(true)
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: bool(false)
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: string(0) ""
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: string(6) "string"
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: string(1) "0"
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: string(3) "100"
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: string(3) "0.0"
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: string(6) "-23.45"
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: object(stdClass)#1 (0) {
}
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Test for: object(stdClass)#2 (0) {
}
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
