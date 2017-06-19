--TEST--
Test readline_read_history function : wrong parameter (array)
- test line 412 from readline.c
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php if (!extension_loaded("readline_info") || !function_exists('readline_read_history') die("skip"); ?>
--FILE--
<?php
$wrong_parameter = array();
$line = readline_read_history($wrong_parameter);
?>
--EXPECTF--
Warning: readline_read_history() expects parameter 1 to be a valid path, array given in %s on line %d
