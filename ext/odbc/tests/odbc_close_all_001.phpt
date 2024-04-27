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

try {
    odbc_columns($conn1, '', '', '', '');
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    odbc_columns($conn2, '', '', '', '');
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    odbc_num_rows($result1);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    odbc_num_rows($result2);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
object(Odbc\Connection)#%d (%d) {
}
object(Odbc\Connection)#%d (%d) {
}
object(Odbc\Result)#%d (%d) {
}
object(Odbc\Result)#%d (%d) {
}
ODBC connection has already been closed
ODBC connection has already been closed
ODBC result has already been closed
ODBC result has already been closed
