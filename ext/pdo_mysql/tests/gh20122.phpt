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
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->exec('CREATE TABLE test_gh20122 (bar JSON)');
$db->exec('INSERT INTO test_gh20122 VALUES("[]")');

$stmt = $db->query('SELECT * from test_gh20122');
$meta = $stmt->getColumnMeta(0);

// Note: JSON is an alias for LONGTEXT on MariaDB!
echo $meta['native_type'], "\n";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->query('DROP TABLE IF EXISTS test_gh20122');
?>
--EXPECTF--
%r(JSON|LONGTEXT)%r
