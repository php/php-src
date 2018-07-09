--TEST--
cli_get_process_title() function : basic functionality
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
if (cli_set_process_title("title") && cli_get_process_title() === "title")
  echo "Title correctly retrieved!\n";

?>
--EXPECT--
Title correctly retrieved!
