--TEST--
free nothing (e.g. no mysqli_close())
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

$result2 = mysqli_query($link, "SELECT CURRENT_USER()");
$stmt2 = mysqli_prepare($link, "SELECT CURRENT_USER()");
printf("Ok\n");
?>
--EXPECT--
Ok
