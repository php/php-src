--TEST--
cli_get_process_title() function : error conditions
--CREDITS--
Patrick Allaert patrickallaert@php.net
@nephp #nephp17
--SKIPIF--
<?php
if (PHP_SAPI !== "cli")
  die("skip");
?>
--FILE--
<?php
cli_get_process_title("foo");
?>
--EXPECTF--
Warning: cli_get_process_title() expects exactly 0 parameters, 1 given in %scli_get_process_title_error.php on line 2
