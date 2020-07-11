--TEST--
MySQL PDOStatement->rowCount() @ SELECT
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();
    MySQLPDOTest::createTestTable($db);

    try {

        if (0 !== ($tmp = $db->query('SELECT id FROM test WHERE 1 = 0')->rowCount()))
            printf("[002] Expecting 0 got %s", var_export($tmp, true));

        if (1 !== ($tmp = $db->query('SELECT id FROM test WHERE id = 1')->rowCount()))
            printf("[003] Expecting 1 got %s", var_export($tmp, true));

    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
done!
