--TEST--
Bug #78152: PDO::exec() - Bad error handling with multiple commands
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php

require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
MySQLPDOTest::createTestTable($db);

var_dump($db->exec("INSERT INTO test(id, label) VALUES (41, 'x'); INSERT INTO test_bad(id, label) VALUES (42, 'y')"));
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
try {
    var_dump($db->exec("INSERT INTO test(id, label) VALUES (42, 'x'); INSERT INTO test_bad(id, label) VALUES (43, 'y')"));
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}

?>
--CLEAN--
<?php
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Warning: PDO::exec(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.test_bad' doesn't exist in %s on line %d
bool(false)
SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.test_bad' doesn't exist
