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
var_dump(readline_callback_handler_install('testing: ', 'foobar!'));
var_dump(readline_callback_handler_install('testing: '));

?>
--EXPECTF--
%Atesting: bool(true)

Warning: readline_callback_handler_install(): foobar! is not callable in %s on line %d
bool(false)

Warning: readline_callback_handler_install() expects exactly 2 parameters, 1 given in %s on line %d
NULL
