--TEST--
GH-20122 (getColumnMeta() for JSON-column in MySQL)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();

require __DIR__ . '/mysql_pdo_test.inc';
if (!MySQLPDOTest::isPDOMySQLnd()) die('skip only for mysqlnd');
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

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
