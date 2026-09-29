--TEST--
MySQL PDOStatement->columnCount() is 0 for the trailing OK packet of a stored procedure
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
$db->exec('DROP PROCEDURE IF EXISTS pdo_mysql_stmt_nextrowset_columncount_p');
$db->exec('CREATE PROCEDURE pdo_mysql_stmt_nextrowset_columncount_p() BEGIN SELECT 1 AS a; SELECT 2 AS b, 3 AS c; END');

foreach ([true, false] as $emulate) {
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, $emulate);
    $stmt = $db->prepare('CALL pdo_mysql_stmt_nextrowset_columncount_p()');
    $stmt->execute();
    do {
        var_dump($stmt->columnCount());
        $stmt->fetchAll();
    } while ($stmt->nextRowset());
}
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP PROCEDURE IF EXISTS pdo_mysql_stmt_nextrowset_columncount_p');
?>
--EXPECT--
int(1)
int(2)
int(0)
int(1)
int(2)
int(0)
