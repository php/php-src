--TEST--
MySQL PDO->lastInsertId()
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    try {
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[001] No query has been run, lastInsertId() should return '0'/string got '%s'/%s\n",
                var_export($tmp, true), gettype($tmp));

        if ('0' !== ($tmp = $db->lastInsertId('sequence_name')))
            printf("[002] MySQL does not support sequences, expecting '0'/string got '%s'/%s\n",
                var_export($tmp, true), gettype($tmp));

        $db->exec('DROP TABLE IF EXISTS test');
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[003] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        $db->exec(sprintf('CREATE TABLE test(id INT, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[004] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        $stmt = $db->query('SELECT id FROM test LIMIT 1');
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[005] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        // no auto increment column
        $db->exec("INSERT INTO test(id, col1) VALUES (100, 'a')");
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[006] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        $db->exec('ALTER TABLE test MODIFY id INT AUTO_INCREMENT PRIMARY KEY');
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[006] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        // duplicate key
        @$db->exec("INSERT INTO test(id, col1) VALUES (100, 'a')");
        if ('0' !== ($tmp = $db->lastInsertId()))
            printf("[007] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        $db->exec("INSERT INTO test(id, col1) VALUES (101, 'b')");
        if ('101' !== ($tmp = $db->lastInsertId()))
            printf("[008] Expecting '0'/string got '%s'/%s", var_export($tmp, true), gettype($tmp));

        $db->exec('ALTER TABLE test MODIFY col1 CHAR(10) UNIQUE');
        // replace = delete + insert -> new auto increment value
        $db->exec("REPLACE INTO test(col1) VALUES ('b')");
        $next_id = (int)$db->lastInsertId();

        if ($next_id <= 101)
            printf("[009] Expecting at least 102, got %d\n",$next_id);

        $stmt = $db->query('SELECT LAST_INSERT_ID() as _last_id');
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
        $last_id = $row['_last_id'];
        if ($next_id != $last_id) {
            printf("[010] LAST_INSERT_ID() = %d and lastInserId() = %d differ\n",
                $last_id, $next_id);
        }

        $db->exec("INSERT INTO test(col1) VALUES ('c'), ('d'), ('e')");
        $next_id = (int)$db->lastInsertId();
        if ($next_id <= $last_id)
            printf("[011] Expecting at least %d, got %d\n", $last_id + 1, $next_id);

        // warnings are unhandy, lets go for exceptions for a second
        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        try {
            $ignore_exception = true;
            $db->exec('LOCK TABLE test WRITE');
            $ignore_exception = false;

            if (MySQLPDOTest::getServerVersion($db) >= 50000) {
                $stmt = $db->query('SELECT @@auto_increment_increment AS inc');
                $row = $stmt->fetch(PDO::FETCH_ASSOC);
                $inc = $row['inc'];
            } else {
                $inc = 1;
            }

            $stmt = $db->query('SELECT LAST_INSERT_ID() as _last_id');
            $row = $stmt->fetch(PDO::FETCH_ASSOC);
            $last_id = $row['_last_id'];

            $db->exec("INSERT INTO test(col1) VALUES ('z')");
            $next_id = (int)$db->lastInsertId();
            if ($next_id < ($last_id + $inc))
                printf("[012] Expecting at least %d, got %d\n", $last_id + $inc, $next_id);

        } catch (PDOException $e) {
            if (!$ignore_exception)
                printf("[014] %s, [%s} %s\n", $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
        }
        $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        @$db->exec('UNLOCK TABLE test');

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
