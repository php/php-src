--TEST--
mysqli_stmt_execute() after mysqli_close()
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

$stmt2 = mysqli_prepare($link, "SELECT CURRENT_USER()");

mysqli_close($link);
try {
    mysqli_stmt_execute($stmt2);
} catch (mysqli_sql_exception $e) {
    echo $e->getMessage(), PHP_EOL;
}
mysqli_stmt_close($stmt2);
printf("Ok\n");
?>
--EXPECT--
MySQL server has gone away
Ok
