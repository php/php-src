--TEST--
Test is_countable() function
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
var_dump(is_countable([1, 2, 3]));
var_dump(is_countable((array) 1));
var_dump(is_countable((object) ['foo', 'bar', 'baz']));
var_dump(is_countable());

$foo = ['', []];

if (is_countable($foo)) {
    var_dump(count($foo));
}

$bar = null;
if (!is_countable($bar)) {
    count($bar);
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)

Warning: is_countable() expects exactly 1 parameter, 0 given in %s on line %d
NULL
int(2)

Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
