--TEST--
non freed statement (missing mysqli_stmt_close())
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

$stmt = mysqli_prepare($link, "SELECT CURRENT_USER()");
mysqli_stmt_execute($stmt);

mysqli_close($link);
printf("Ok\n");
?>
--EXPECT--
Ok
