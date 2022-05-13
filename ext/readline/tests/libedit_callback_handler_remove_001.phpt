--TEST--
readline_callback_handler_remove(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_callback_handler_remove')) die("skip");
if (READLINE_LIB != "libedit") die("skip libedit only");
?>
--INI--
zend.signal_check=0
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
