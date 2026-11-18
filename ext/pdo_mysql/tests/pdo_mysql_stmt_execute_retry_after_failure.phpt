--TEST--
PDO MySQL native prepared statement can retry after execute failure
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
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$db->exec('CREATE TABLE pdo_mysql_stmt_execute_retry (id INT PRIMARY KEY, value INT NOT NULL)');

$stmt = $db->prepare('INSERT INTO pdo_mysql_stmt_execute_retry (id, value) VALUES (1, ?)');
$value = null;
$stmt->bindParam(1, $value, PDO::PARAM_INT);

try {
    $stmt->execute();
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$value = 2;
var_dump($stmt->execute());

$stmt = $db->prepare('INSERT INTO pdo_mysql_stmt_execute_retry (id, value) VALUES (?, ?)');
$stmt->bindValue(1, 2, PDO::PARAM_INT);
$value = null;
$stmt->bindParam(2, $value, PDO::PARAM_STR);

try {
    $stmt->execute();
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$value = '3';
var_dump($stmt->execute());
foreach ($db->query('SELECT id, value FROM pdo_mysql_stmt_execute_retry ORDER BY id') as $row) {
    echo $row['id'], ': ', $row['value'], PHP_EOL;
}
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_stmt_execute_retry');
?>
--EXPECT--
PDOException: SQLSTATE[23000]: Integrity constraint violation: 1048 Column 'value' cannot be null
bool(true)
PDOException: SQLSTATE[23000]: Integrity constraint violation: 1048 Column 'value' cannot be null
bool(true)
1: 2
2: 3
