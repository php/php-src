--TEST--
odbc_setoption(): Test for odbc_setoption() with persistent connection
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_pconnect($dsn, $user, $pass);
odbc_autocommit($conn, true);

var_dump(odbc_setoption($conn, 1, 102 /* autocommit */, 0));

var_dump(odbc_autocommit($conn));

odbc_close($conn);

?>
--EXPECTF--
Warning: odbc_setoption(): Unable to set option for persistent connection in %s on line %d
bool(false)
int(1)
