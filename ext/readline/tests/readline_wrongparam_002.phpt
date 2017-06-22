--TEST--
Check the function readline with 2 parameters (wrong because the function expects only one parameter).
- lines from 221 to 223 - readline.c
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php if (!extension_loaded("readline") die("skip"); ?>
--FILE--
<?php
$line = readline("Prompt", true);
?>
--EXPECTF--
Warning: readline() expects at most 1 parameter, 2 given in %s on line %d
