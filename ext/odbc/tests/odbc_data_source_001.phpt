--TEST--
odbc_data_source(): Basic test
--SKIPIF--
<?php
	include 'skipif.inc';
	if (odbc_data_source($conn, SQL_FETCH_FIRST) === NULL) {
		die("skip no data sources defined on this system");
	}
?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

var_dump(odbc_data_source($conn, NULL));
var_dump(odbc_data_source($conn, ''));
var_dump(odbc_data_source($conn, SQL_FETCH_FIRST));

?>
--EXPECTF--
Warning: odbc_data_source(): Invalid fetch type (0) in %s on line %d
bool(false)

Warning: odbc_data_source() expects parameter 2 to be int, string given in %s on line %d
NULL
array(%d) {
%a
}
