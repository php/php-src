--TEST--
cli_get_process_title() function : basic functionality
--CREDITS--
Patrick Allaert patrickallaert@php.net
@nephp #nephp17
--FILE--
<?php
if (cli_set_process_title("title") && cli_get_process_title() === "title")
  echo "Title correctly retrieved!\n";

?>
--EXPECT--
Title correctly retrieved!
