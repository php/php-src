--TEST--
free resultset (mysqli_free_result()) after mysqli_close()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

$link = default_mysqli_connect();

$result1 = mysqli_query($link, "SELECT CURRENT_USER()");
mysqli_close($link);
mysqli_free_result($result1);
printf("Ok\n");
?>
--EXPECT--
Ok
