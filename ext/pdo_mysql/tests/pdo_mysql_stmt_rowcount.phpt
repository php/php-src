--TEST--
MySQL PDOStatement->rowCount() @ SELECT
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

    $table = 'pdo_mysql_stmt_rowcount';
    MySQLPDOTest::createTestTable($table, $db);

    try {

        if (0 !== ($tmp = $db->query("SELECT id FROM {$table} WHERE 1 = 0")->rowCount()))
            printf("[002] Expecting 0 got %s", var_export($tmp, true));

        if (1 !== ($tmp = $db->query("SELECT id FROM {$table} WHERE id = 1")->rowCount()))
            printf("[003] Expecting 1 got %s", var_export($tmp, true));

    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_stmt_rowcount');
?>
--EXPECT--
done!
