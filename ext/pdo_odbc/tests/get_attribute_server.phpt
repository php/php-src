--TEST--
PDO ODBC getAttribute SERVER_INFO and SERVER_VERSION
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
ODBCPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
// Obviously, we can't assume what driver is being used, so just check strings
// Example driver output (MariaDB ODBC):
// PDO::ATTR_SERVER_INFO: MariaDB
// PDO::ATTR_SERVER_VERSION: 10.04.000018
// As well as IBM i ODBC:
// PDO::ATTR_SERVER_INFO: DB2/400 SQL
// PDO::ATTR_SERVER_VERSION: 07.02.0015
var_dump($db->getAttribute(PDO::ATTR_SERVER_INFO));
var_dump($db->getAttribute(PDO::ATTR_SERVER_VERSION));
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
