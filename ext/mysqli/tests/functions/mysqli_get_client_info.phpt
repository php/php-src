--TEST--
function test: mysqli_get_client_info
--EXTENSIONS--
mysqli
--FILE--
<?php
// As of PHP 8.2.0 cannot be linked against libmysql anymore
$expected_version = 'mysqlnd ' . PHP_VERSION;
var_dump(mysqli_get_client_info() === $expected_version);
?>
--EXPECT--
bool(true)
