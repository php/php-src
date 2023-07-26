--TEST--
odbc_close_all(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn1 = odbc_connect($dsn, $user, $pass);
$conn2 = odbc_pconnect($dsn, $user, $pass);
$result1 = odbc_columns($conn1, '', '', '', '');
$result2 = odbc_columns($conn2, '', '', '', '');

var_dump($conn1);
var_dump($conn2);
var_dump($result1);
var_dump($result2);

odbc_close_all();

var_dump($conn1);
var_dump($conn2);
var_dump($result1);
var_dump($result2);

?>
--EXPECTF--
resource(5) of type (odbc link)
resource(7) of type (odbc link persistent)
resource(8) of type (odbc result)
resource(9) of type (odbc result)
resource(5) of type (Unknown)
resource(7) of type (Unknown)
resource(8) of type (Unknown)
resource(9) of type (Unknown)
