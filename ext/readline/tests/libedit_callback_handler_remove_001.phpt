--TEST--
readline_callback_handler_remove(): Basic test
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_callback_handler_remove')) die("skip");
if (READLINE_LIB != "libedit") die("skip libedit only");
?>
--FILE--
<?php

var_dump(readline_callback_handler_remove());
var_dump(readline_callback_handler_install('testing: ', 'foo'));

function foo() { }
var_dump(readline_callback_handler_install('testing: ', 'foo'));
var_dump(readline_callback_handler_remove());

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
