--TEST--
Bug #80783 (PDO ODBC truncates BLOB records at every 256th byte)
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

$table_name = 'bug80783_pdo_odbc';

$db->exec("CREATE TABLE {$table_name} (name IMAGE)");

$string = str_repeat("0123456789", 50);
$db->exec("INSERT INTO {$table_name} VALUES('$string')");

$stmt = $db->prepare("SELECT name FROM {$table_name}");
$stmt->bindColumn(1, $data, PDO::PARAM_LOB);
$stmt->execute();
$stmt->fetch(PDO::FETCH_BOUND);

var_dump($data === bin2hex($string));
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS bug80783_pdo_odbc");
?>
--EXPECT--
bool(true)
