--TEST--
PDO::ATTR_AUTOCOMMIT
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

    // autocommit should be on by default
    if (true !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
        printf("[001] Expecting int/1 got %s\n", var_export($tmp, true));

    // lets see if the server agrees to that
    $row = $db->query('SELECT @@autocommit AS _autocommit')->fetch(PDO::FETCH_ASSOC);
    if (!$row['_autocommit'])
        printf("[002] Server autocommit mode should be on, got '%s'\n", var_export($row['_autocommit']));

    // on -> off
    if (!$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0))
        printf("[003] Cannot turn off autocommit\n");

    $row = $db->query('SELECT @@autocommit AS _autocommit')->fetch(PDO::FETCH_ASSOC);
    if ($row['_autocommit'])
        printf("[004] Server autocommit mode should be off, got '%s'\n", var_export($row['_autocommit']));

    // PDO thinks autocommit is off, but its manually turned on...
    if (!$db->query('SET autocommit = 1'))
        printf("[005] Cannot turn on server autocommit mode, %s\n", var_export($db->errorInfo(), true));

    if (false !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
        printf("[006] Expecting int/0 got %s\n", var_export($tmp, true));

    // off -> on
    if (!$db->query('SET autocommit = 0'))
        printf("[007] Cannot turn off server autocommit mode, %s\n", var_export($db->errorInfo(), true));

    if (!$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 1))
        printf("[008] Cannot turn on autocommit\n");

    $row = $db->query('SELECT @@autocommit AS _autocommit')->fetch(PDO::FETCH_ASSOC);
    if (!$row['_autocommit'])
        printf("[009] Server autocommit mode should be on, got '%s'\n", var_export($row['_autocommit']));

    if (true !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
        printf("[010] Expecting int/1 got %s\n", var_export($tmp, true));

    $table = 'pdo_mysql_attr_autocommit';
    if (MySQLPDOTest::detect_transactional_mysql_engine($db)) {
        // nice, we have a transactional engine to play with

        MySQLPDOTest::createTestTable($table, $db, MySQLPDOTest::detect_transactional_mysql_engine($db));
        $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
        $num = $row['_num'];

        $db->query("INSERT INTO {$table} (id, label) VALUES (100, 'z')");
        $num++;
        $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
        if ($row['_num'] != $num)
            printf("[011] Insert has failed, test will fail\n");

        // autocommit is on, no rollback possible
        $db->query('ROLLBACK');
        $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
        if ($row['_num'] != $num)
            printf("[012] ROLLBACK should not have undone anything\n");

        if (!$db->setAttribute(PDO::ATTR_AUTOCOMMIT, 0))
            printf("[013] Cannot turn off autocommit\n");

        $db->query("DELETE FROM {$table} WHERE id = 100");
        $db->query('ROLLBACK');
        $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
        if ($row['_num'] != $num)
            printf("[014] ROLLBACK should have undone the DELETE\n");

        $db->query("DELETE FROM {$table} WHERE id = 100");
        $db->query('COMMIT');
        $num--;
        $row = $db->query("SELECT COUNT(*) AS _num FROM {$table}")->fetch(PDO::FETCH_ASSOC);
        if ($row['_num'] != $num)
            printf("[015] DELETE should have been committed\n");
    }

    print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS pdo_mysql_attr_autocommit');
?>
--EXPECT--
done!
