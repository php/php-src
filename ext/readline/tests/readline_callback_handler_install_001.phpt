--TEST--
readline_callback_handler_install(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_callback_handler_install')) die("skip");
if (READLINE_LIB == "libedit") die("skip readline only");
?>
--FILE--
<?php

function foo() {
    readline_callback_handler_remove();
}

var_dump(readline_callback_handler_install('testing: ', 'foo'));
try {
    var_dump(readline_callback_handler_install('testing: ', 'foobar!'));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
testing: bool(true)
readline_callback_handler_install(): Argument #2 ($callback) must be a valid callback, function "foobar!" not found or invalid function name
