--TEST--
MySQL Prepared Statements and BLOBs
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

    $blobs = array(
        'TINYBLOB'		=> 255,
        'TINYTEXT'		=> 255,
        'BLOB'				=> 32767,
        'TEXT'				=> 32767,
        'MEDIUMBLOB'	=> 100000,
        'MEDIUMTEXT'	=> 100000,
        'LONGBLOB'		=> 100000,
        'LONGTEXT'		=> 100000,
    );

    function test_blob($db, $offset, $sql_type, $test_len) {

        $db->exec('DROP TABLE IF EXISTS test_stmt_blobs');
        $db->exec(sprintf('CREATE TABLE test_stmt_blobs(id INT, label %s) ENGINE=%s', $sql_type, PDO_MYSQL_TEST_ENGINE));

        $value = str_repeat('a', $test_len);
        $stmt = $db->prepare('INSERT INTO test_stmt_blobs(id, label) VALUES (?, ?)');
        $stmt->bindValue(1, 1);
        $stmt->bindValue(2, $value);
        if (!$stmt->execute()) {
            printf("[%03d + 1] %d %s\n",
                $offset, $stmt->errorCode(), var_export($stmt->errorInfo(), true));
            return false;
        }

        $stmt = $db->query('SELECT id, label FROM test_stmt_blobs');
        $id = $label = NULL;
        $stmt->bindColumn(1, $id, PDO::PARAM_INT);
        $stmt->bindColumn(2, $label, PDO::PARAM_LOB);

        if (!$stmt->fetch(PDO::FETCH_BOUND)) {
            printf("[%03d + 2] %d %s\n",
                $offset, $stmt->errorCode(), var_export($stmt->errorInfo(), true));
            return false;
        }

        if (!is_resource($label)) {
            printf("[%03d + 3] Returned value is not a stream resource\n", $offset);
            return false;
        }

        $contents = stream_get_contents($label);
        if ($contents !== $value) {
            printf("[%03d + 3] Returned value seems to be wrong (%d vs. %d characters). Check manually\n",
                $offset, strlen($contents), strlen($value));
            return false;
        }

        if (1 != $id) {
            printf("[%03d + 3] Returned id column value seems wrong, expecting 1 got %s.\n",
                $offset, var_export($id, true));
            return false;
        }

        $stmt = $db->query('SELECT id, label FROM test_stmt_blobs');
        $ret = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($ret['label'] !== $value) {
            printf("[%03d + 3] Returned value seems to be wrong (%d vs. %d characters). Check manually\n",
                $offset, strlen($ret['label']), strlen($value));
            return false;
        }

        if (1 != $ret['id']) {
            printf("[%03d + 3] Returned id column value seems wrong, expecting 1 got %s.\n",
                $offset, var_export($ret['id'], true));
            return false;
        }

        return true;
    }

    $offset = 0;
    foreach ($blobs as $sql_type => $test_len) {
        $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
        test_blob($db, ++$offset, $sql_type, $test_len);
        $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
        test_blob($db, ++$offset, $sql_type, $test_len);
    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test_stmt_blobs');
?>
--EXPECT--
done!
