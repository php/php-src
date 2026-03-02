--TEST--
odbc_setoption(): Basic test for odbc_setoption()
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_autocommit($conn, true);

var_dump(odbc_setoption($conn, 1, 102 /* autocommit */, 0));

var_dump(odbc_autocommit($conn));

odbc_close($conn);

?>
--EXPECT--
bool(true)
int(0)
