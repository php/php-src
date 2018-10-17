<?php

putenv('ODBCINI=/etc/odbc.ini');
putenv('ODBCSYSINI=/etc');

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
