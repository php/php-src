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
resource(%d) of type (odbc link)
resource(%d) of type (odbc link persistent)
resource(%d) of type (odbc result)
resource(%d) of type (odbc result)
resource(%d) of type (Unknown)
resource(%d) of type (Unknown)
resource(%d) of type (Unknown)
resource(%d) of type (Unknown)
