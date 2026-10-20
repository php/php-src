--TEST--
readline_completion_function(): Basic test
--EXTENSIONS--
readline
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
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
bool(true)
bool(true)
TypeError: readline_completion_function(): Argument #1 ($callback) must be a valid callback, no array or string given
TypeError: readline_completion_function(): Argument #1 ($callback) must be a valid callback, no array or string given
