--TEST--
odbc_columnprivileges(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

var_dump($result = odbc_columnprivileges($conn, '', '', '', ''));
var_dump(odbc_fetch_row($result));

var_dump($result = odbc_columnprivileges($conn, NULL, NULL, NULL, NULL));
var_dump(odbc_fetch_row($result));

var_dump($result = odbc_columnprivileges($conn, 'FOO', 'FOO', 'FOO', 'FOO'));
var_dump(odbc_fetch_row($result));

?>
--EXPECTF--
resource(%d) of type (odbc result)
bool(false)

Deprecated: odbc_columnprivileges(): Passing null to parameter #3 ($schema) of type string is deprecated in %s on line %d

Deprecated: odbc_columnprivileges(): Passing null to parameter #4 ($table) of type string is deprecated in %s on line %d

Deprecated: odbc_columnprivileges(): Passing null to parameter #5 ($column) of type string is deprecated in %s on line %d
resource(%d) of type (odbc result)
bool(false)
resource(%d) of type (odbc result)
bool(false)
