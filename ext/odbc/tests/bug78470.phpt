--TEST--
Bug #78470 (odbc_specialcolumns() no longer accepts $nullable)
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
var_dump(odbc_specialcolumns($conn, SQL_BEST_ROWID, '', '', '', SQL_SCOPE_CURROW, SQL_NO_NULLS));
?>
--EXPECTF--
resource(%d) of type (odbc result)
