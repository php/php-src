--TEST--
not freed resultset (missing mysqli_free_result())
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

$result = mysqli_query($link, "SELECT CURRENT_USER()");
mysqli_close($link);
printf("Ok\n");

?>
--EXPECT--
Ok
