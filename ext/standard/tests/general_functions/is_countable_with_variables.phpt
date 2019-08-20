--TEST--
Test is_countable() function
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
var_dump(is_countable([1, 2, 3]));
var_dump(is_countable((array) 1));

try {
    var_dump(is_countable((object) ['foo', 'bar', 'baz']));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

$foo = ['', []];

if (is_countable($foo)) {
    var_dump(count($foo));
}

$bar = null;
if (!is_countable($bar)) {
    count($bar);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
int(2)
Parameter must be an array or an object that implements Countable
