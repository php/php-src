--TEST--
readline_callback_handler_remove(): Basic test
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_callback_handler_remove')) die("skip");
if (READLINE_LIB == "libedit") die("skip readline only");
?>
--FILE--
<?php

var_dump(readline_callback_handler_remove());
var_dump(readline_callback_handler_install('testing: ', 'foo'));

function foo() { }
var_dump(readline_callback_handler_install('testing: ', 'foo'));
var_dump(readline_callback_handler_remove());

?>
--EXPECTF--
bool(false)
%Atesting: bool(true)
testing: bool(true)
bool(true)
