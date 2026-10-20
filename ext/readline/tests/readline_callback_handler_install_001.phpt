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

?>
--EXPECT--
testing: bool(true)
