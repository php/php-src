--TEST--
cli_set_process_title() function : basic functionality
--CREDITS--
Patrick Allaert patrickallaert@php.net
@nephp #nephp17
--SKIPIF--
<?php
if (PHP_SAPI !== "cli")
  die("skip cli process title not available in non-cli SAPI");
if (!PHP_CLI_PROCESS_TITLE)
  die("skip process title not available (disabled or unsupported)");
?>
--FILE--
<?php
if (cli_set_process_title("title") === true &&
    cli_get_process_title() === "title")
  echo "Successfully set title\n";

?>
--EXPECT--
Successfully set title
