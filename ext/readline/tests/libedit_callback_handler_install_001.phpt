--TEST--
readline_callback_handler_install(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_callback_handler_install')) die("skip");
if (READLINE_LIB != "libedit") die("skip libedit only");
?>
--FILE--
<?php

function foo() {
	readline_callback_handler_remove();
}

var_dump(readline_callback_handler_install('testing: ', 'foo'));
var_dump(readline_callback_handler_install('testing: ', 'foobar!'));

?>
--EXPECTF--
bool(true)

Warning: readline_callback_handler_install(): foobar! is not callable in %s on line %d
bool(false)
