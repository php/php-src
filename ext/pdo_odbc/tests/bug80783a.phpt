--TEST--
Bug #80783 (PDO ODBC truncates BLOB records at every 256th byte)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
ODBCPDOTest::skipWithUnixODBC();
ODBCPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();

$table_name = 'bug80783a_pdo_odbc';

$db->exec("CREATE TABLE {$table_name} (name NVARCHAR(MAX))");

$string = str_repeat("0123456789", 50);
$db->exec("INSERT INTO {$table_name} VALUES('$string')");

$stmt = $db->prepare("SELECT name FROM {$table_name}");
$stmt->setAttribute(PDO::ODBC_ATTR_ASSUME_UTF8, true);
$stmt->bindColumn(1, $data, PDO::PARAM_STR);
$stmt->execute();
$stmt->fetch(PDO::FETCH_BOUND);

var_dump($data === $string);
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS bug80783a_pdo_odbc");
?>
--EXPECT--
bool(true)
