--TEST--
MySQL PDO->commit()
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

    $table = 'pdo_mysql_commit';
    MySQLPDOTest::createTestTable($table, $db, MySQLPDOTest::detect_transactional_mysql_engine($db));

    $table2 = 'pdo_mysql_commit_2';
    try {
        if (true !== ($tmp = $db->beginTransaction())) {
            printf("[001] Expecting true, got %s/%s\n", gettype($tmp), $tmp);
        }

        // DDL will issue an implicit commit
        $db->exec(sprintf('CREATE TABLE %s (id INT) ENGINE=%s', $table2, MySQLPDOTest::detect_transactional_mysql_engine($db)));
        try {
            $db->commit();
            $failed = false;
        } catch (PDOException $e) {
            $failed = true;
        }
        if (!$failed) {
            printf("[002] Commit should have failed\n");
        }

        // pdo_transaction_transitions should check this as well...
        // ... just to be sure the most basic stuff really works we check it again...
        if (1 !== ($tmp = $db->getAttribute(PDO::ATTR_AUTOCOMMIT)))
            printf("[003] According to the manual we should be back to autocommit mode, got %s/%s\n",
                gettype($tmp), var_export($tmp, true));

        if (true !== ($tmp = $db->beginTransaction()))
            printf("[004] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

        $db->exec("INSERT INTO {$table} (id, label) VALUES (100, 'z')");

        if (true !== ($tmp = $db->commit()))
            printf("[005] No commit allowed? [%s] %s\n",
                $db->errorCode(), implode(' ', $db->errorInfo()));

        // a weak test without unicode etc. - lets leave that to dedicated tests
        $stmt = $db->query("SELECT id, label FROM {$table} WHERE id = 100");
        $rows = $stmt->fetchAll(PDO::FETCH_ASSOC);
        if (!isset($rows[0]['label']) || ($rows[0]['label'] != 'z')) {
            printf("[006] Record data is strange, dumping rows\n");
            var_dump($rows);
        }

        // Ok, lets check MyISAM resp. any other non-transactional engine
        // pdo_mysql_begin_transaction has more on this, quick check only
        $db = MySQLPDOTest::factory();
        MySQLPDOTest::createTestTable($table, $db, 'MyISAM');

        if (true !== ($tmp = $db->beginTransaction()))
            printf("[007] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

        $db->exec("INSERT INTO {$table} (id, label) VALUES (100, 'z')");
        if (true !== ($tmp = $db->commit()))
            printf("[008] No commit allowed? [%s] %s\n",
                $db->errorCode(), implode(' ', $db->errorInfo()));

        // a weak test without unicode etc. - lets leave that to dedicated tests
        $stmt = $db->query("SELECT id, label FROM {$table} WHERE id = 100");
        $rows = $stmt->fetchAll(PDO::FETCH_ASSOC);
        if (!isset($rows[0]['label']) || ($rows[0]['label'] != 'z')) {
            printf("[009] Record data is strange, dumping rows\n");
            var_dump($rows);
        }

    } catch (PDOException $e) {
        printf("[002] %s, [%s] %s\n",
            $e->getMessage(),
            $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_commit');
$db->exec('DROP TABLE IF EXISTS pdo_mysql_commit_2');
?>
--EXPECT--
done!
