--TEST--
readline_callback_handler_install(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_callback_handler_install')) die("skip");
if (READLINE_LIB == "libedit") die("skip readline only");
?>
--FILE--
<?php

function foo() {
    readline_callback_handler_remove();
}

var_dump(readline_callback_handler_install('testing: ', 'foo'));

try {
    readline_callback_handler_install('testing: ', 'foobar!');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
%Atesting: bool(true)
readline_callback_handler_install(): Argument #2 ($callback) must be of type callable, string given
