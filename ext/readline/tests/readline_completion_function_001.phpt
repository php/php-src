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
        readline_completion_function($callback);
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
readline_completion_function(): Argument #1 ($funcname) must be of type callable, int given
readline_completion_function(): Argument #1 ($funcname) must be of type callable, float given
