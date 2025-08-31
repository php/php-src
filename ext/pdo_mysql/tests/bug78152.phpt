--TEST--
Bug #78152: PDO::exec() - Bad error handling with multiple commands
--EXTENSIONS--
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

$table = 'bug78152_pdo_mysql';
MySQLPDOTest::createTestTable($table, $db);

var_dump($db->exec("INSERT INTO {$table} (id, label) VALUES (41, 'x'); INSERT INTO {$table}_bad (id, label) VALUES (42, 'y')"));
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
try {
    var_dump($db->exec("INSERT INTO {$table} (id, label) VALUES (42, 'x'); INSERT INTO {$table}_bad (id, label) VALUES (43, 'y')"));
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS bug78152_pdo_mysql');
?>
--EXPECTF--
Warning: PDO::exec(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.bug78152_pdo_mysql_bad' doesn't exist in %s on line %d
bool(false)
SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.bug78152_pdo_mysql_bad' doesn't exist
