--TEST--
GH-20122 (getColumnMeta() for JSON-column in MySQL)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--XFAIL--
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();

$db->exec('CREATE TABLE test (bar JSON)');
$db->exec('INSERT INTO test VALUES("[]")');

$stmt = $db->query('SELECT * from test');
$meta = $stmt->getColumnMeta(0);

// Note: JSON is an alias for LONGTEXT on MariaDB!
echo $meta['native_type'], "\n";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
%r(JSON|LONGTEXT)%r
