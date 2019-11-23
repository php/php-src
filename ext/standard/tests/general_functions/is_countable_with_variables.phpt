--TEST--
Test is_countable() function
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
var_dump(is_countable([1, 2, 3]));
var_dump(is_countable((array) 1));
var_dump(is_countable((object) ['foo', 'bar', 'baz']));

$foo = ['', []];

if (is_countable($foo)) {
    var_dump(count($foo));
}

$bar = null;
if (!is_countable($bar)) {
    var_dump(count($bar));
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
int(2)

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)
