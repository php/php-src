--TEST--
Test readline function : wrong parameter (array)
- lines from 221 to 223 - readline.c
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php if (!extension_loaded("readline") die("skip"); ?>
--FILE--
<?php
$wrong_parameter = array();
$line = readline($wrong_parameter);
?>
--EXPECTF--
Warning: readline() expects parameter 1 to be string, array given in %s on line %d
