--TEST--
free statement (mysqli_stmt_close()) after mysqli_close()
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

$stmt1 = mysqli_prepare($link, "SELECT CURRENT_USER()");
mysqli_stmt_execute($stmt1);

mysqli_close($link);
mysqli_stmt_close($stmt1);
printf("Ok\n");
?>
--EXPECT--
Ok
