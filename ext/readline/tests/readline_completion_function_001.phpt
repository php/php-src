--TEST--
readline_completion_function(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); ?>
--FILE--
<?php

function foo() { }

$data = array(
    'foo',
    'strtolower',
    1,
    1.1231
);

foreach ($data as $callback) {
    try {
        var_dump(readline_completion_function($callback));
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--EXPECT--
bool(true)
bool(true)
readline_completion_function(): Argument #1 ($callback) must be a valid callback, no array or string given
readline_completion_function(): Argument #1 ($callback) must be a valid callback, no array or string given
