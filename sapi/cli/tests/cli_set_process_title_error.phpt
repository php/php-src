--TEST--
cli_set_process_title() function : error conditions
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
cli_set_process_title();
cli_set_process_title("foo", "bar");
?>
--EXPECTF--
Warning: cli_set_process_title() expects exactly 1 parameter, 0 given in %scli_set_process_title_error.php on line 2

Warning: cli_set_process_title() expects exactly 1 parameter, 2 given in %scli_set_process_title_error.php on line 3
