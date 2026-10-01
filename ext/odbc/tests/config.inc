<?php

defined('ODBC_SQLITE_DSN') || define('ODBC_SQLITE_DSN', "Driver=SQLite3;Database=:memory:");

$dsn  = getenv("ODBC_TEST_DSN");
$user = getenv("ODBC_TEST_USER");
$pass = getenv("ODBC_TEST_PASS");

if (false === $dsn) {
    $dsn = 'myodbc3';
}
if (false === $user) {
    $user = 'root';
}
if (false == $pass) {
    $pass = '';
}
