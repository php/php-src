--TEST--
readline(): Basic test
--CREDITS--
Mark Railton
mark@markrailton.com
PHP TestFest 2017 - PHPDublin
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip"); 
if (READLINE_LIB != "libedit") die("skip libedit only");
?>
--FILE--
<?php

var_dump(readline());

--EXPECTF--
bool(false)