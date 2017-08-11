--TEST--
cli_set_process_title() function : basic functionality
--CREDITS--
Patrick Allaert patrickallaert@php.net
@nephp #nephp17
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
  die("skip");
?>
--FILE--
<?php
if (cli_set_process_title("title") === true &&
    cli_get_process_title() === "title")
  echo "Successfully set title\n";

?>
--EXPECTF--
Successfully set title
