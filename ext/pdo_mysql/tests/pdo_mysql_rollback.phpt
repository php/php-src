--TEST--
PDO::rollBack()
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotTransactionalEngine();
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();

    $table = 'pdo_mysql_rollback';
    $table2 = 'pdo_mysql_rollback_2';

    MySQLPDOTest::createTestTable($table, $db, MySQLPDOTest::detect_transactional_mysql_engine($db));

    $db->beginTransaction();

    $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
    $num = $row['_num'];

    $db->query("INSERT INTO {$table}(id, label) VALUES (100, 'z')");
    $num++;
    $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
    if ($row['_num'] != $num)
        printf("[001] INSERT has failed, test will fail\n");

    $db->rollBack();
    $num--;
    $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
    if ($row['_num'] != $num)
        printf("[002] ROLLBACK has failed\n");

    $db->beginTransaction();
    $db->query("INSERT INTO {$table}(id, label) VALUES (100, 'z')");
    $db->query("CREATE TABLE {$table2}(id INT)");
    $num++;
    try {
        $db->rollBack();
        $failed = false;
    } catch (PDOException $e) {
        $failed = true;
    }
    if (!$failed) {
        printf("[003] Rollback should have failed\n");
    }

    $db->query("DROP TABLE IF EXISTS {$table2}");
    $db->query("CREATE TABLE {$table2}(id INT) ENGINE=MyISAM");
    $db->beginTransaction();
    $db->query("INSERT INTO {$table2}(id) VALUES (1)");
    $db->rollBack();
    $row = $db->query("SELECT COUNT(*) AS _num FROM {$table2}")->fetch(PDO::FETCH_ASSOC);
    if ($row['_num'] != 1)
        printf("[003] ROLLBACK should have no effect\n");

    $db->query("DROP TABLE IF EXISTS {$table2}");

    $db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
    $db->beginTransaction();
    $db->query("DELETE FROM {$table}");
    $db->rollBack();
    var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

    $db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0);
    $db->beginTransaction();
    $db->query("DELETE FROM {$table}");
    $db->rollBack();
    var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

    $db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1);
    $db->beginTransaction();
    $db->query("DELETE FROM {$table}");
    $db->commit();
    var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

    $db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0);
    $db->beginTransaction();
    $db->query("DELETE FROM {$table}");
    $db->commit();
    var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT));

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_rollback');
$db->exec('DROP TABLE IF EXISTS pdo_mysql_rollback_2');
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
done!
