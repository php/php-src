--TEST--
Test readline_info function : wrong parameter (array)
- line 248 from readline.c
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php if (!extension_loaded("readline")) die("skip");
if (READLINE_LIB == "libedit") die("skip readline only"); ?>
--FILE--
<?php
$wrong_parameter = array();
$info = readline_info($wrong_parameter);
?>
--EXPECTF--
Warning: readline_info() expects parameter 1 to be string, array given in %s on line %d
