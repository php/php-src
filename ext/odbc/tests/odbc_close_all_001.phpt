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
object(ODBC\Connection)#%d (%d) {
}
object(ODBC\Connection)#%d (%d) {
}
object(ODBC\Result)#%d (%d) {
}
object(ODBC\Result)#%d (%d) {
}
object(ODBC\Connection)#%d (%d) {
}
object(ODBC\Connection)#%d (%d) {
}
object(ODBC\Result)#%d (%d) {
}
object(ODBC\Result)#%d (%d) {
}
