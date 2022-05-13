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
    try {
        count($bar);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
int(2)
count(): Argument #1 ($value) must be of type Countable|array, null given
