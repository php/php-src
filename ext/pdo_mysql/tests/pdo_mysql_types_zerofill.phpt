--TEST--
MySQL PDO->exec(), native types - ZEROFILL
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

    function test_type(&$db, $offset, $sql_type, $value, $ret_value = NULL, $pattern = NULL) {

        $db->exec('DROP TABLE IF EXISTS test');
        $sql = sprintf('CREATE TABLE test(id INT, label %s) ENGINE=%s', $sql_type, MySQLPDOTest::getTableEngine());
        @$db->exec($sql);
        if ($db->errorCode() != 0) {
            // not all MySQL Server versions and/or engines might support the type
            return true;
        }

        $stmt = $db->prepare('INSERT INTO test(id, label) VALUES (?, ?)');
        $stmt->bindValue(1, $offset);
        $stmt->bindValue(2, $value);
        try {
            if (!$stmt->execute()) {
                printf("[%03d + 1] INSERT failed, %s\n", $offset, var_export($stmt->errorInfo(), true));
                return false;
            }
        } catch (PDOException $e) {
            // This might be a SQL warning on signed values inserted in unsigned columns
            // Zerofill implies unsigned but the test plays with signed = negative values as well!
            return true;
        }

        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
        $stmt = $db->query('SELECT id, label FROM test');
        $row = $stmt->fetch(PDO::FETCH_ASSOC);
        $stmt->closeCursor();
        if (!isset($row['id']) || !isset($row['label'])) {
            printf("[%03d + 2] Fetched result seems wrong, dumping result: %s\n", $offset, var_export($row, true));
            return false;
        }

        if ($row['id'] != $offset) {
            printf("[%03d + 3] Expecting %s got %s\n", $offset, $row['id']);
            return false;
        }

        if (!is_null($pattern)) {

            if (!preg_match($pattern, $row['label'])) {
                printf("[%03d + 5] Value seems wrong, accepting pattern %s got %s, check manually\n",
                    $offset, $pattern, var_export($row['label'], true));
                return false;
            }

        } else {

            $exp = $value;
            if (!is_null($ret_value)) {
                // we expect a different return value than our input value
                // typically the difference is only the type
                $exp = $ret_value;
            }

            if ($row['label'] !== $exp) {
                printf("[%03d + 4] %s - input = %s/%s, output = %s/%s\n", $offset,
                    $sql_type, var_export($exp, true), gettype($exp),
                    var_export($row['label'], true), gettype($row['label']));
                return false;
            }

        }

        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
        $stmt = $db->query('SELECT id, label FROM test');
        $row_string = $stmt->fetch(PDO::FETCH_ASSOC);
        $stmt->closeCursor();
        if ($row['label'] != $row_string['label']) {
            printf("%s - STRINGIGY = %s, NATIVE = %s\n", $sql_type, var_export($row_string['label'], true), var_export($row['label'], true));
            return false;
        }

        return true;
    }

    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

    $stmt = $db->prepare('SELECT @@sql_mode AS _mode');
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_ASSOC);
    $real_as_float = (false === stristr($row['_mode'], "REAL_AS_FLOAT")) ? false : true;

    test_type($db, 100, 'REAL ZEROFILL', -1.01, NULL, '/^[0]*0$/');
    test_type($db, 110, 'REAL ZEROFILL', 1.01, NULL, ($real_as_float) ? '/^[0]*1\.0.*$/' : '/^[0]*1\.01$/');
    test_type($db, 120, 'REAL UNSIGNED ZEROFILL', 1.01, NULL, ($real_as_float) ? '/^[0]*1\..*$/' : '/^[0]*1\.01$/');

    test_type($db, 130, 'DOUBLE ZEROFILL', -1.01, NULL, '/^[0]*0$/');
    test_type($db, 140, 'DOUBLE ZEROFILL', 1.01, NULL, '/^[0]*1\.01$/');
    test_type($db, 150, 'DOUBLE UNSIGNED ZEROFILL', 1.01, NULL, '/^[0]*1\.01$/');

    test_type($db, 160, 'FLOAT ZEROFILL', -1.01, NULL, '/^[0]*0$/');
    test_type($db, 170, 'FLOAT ZEROFILL', 1, NULL, '/^[0]*1$/');
    test_type($db, 180, 'FLOAT UNSIGNED ZEROFILL', -1, NULL, '/^[0]*0$/');

    test_type($db, 190, 'DECIMAL ZEROFILL', -1.01, NULL, '/^[0]*0$/');
    test_type($db, 200, 'DECIMAL ZEROFILL', 1.01, NULL, '/^[0]*1$/');
    test_type($db, 210, 'DECIMAL UNSIGNED ZEROFILL', 1.01, NULL, '/^[0]*1$/');

    test_type($db, 220, 'NUMERIC ZEROFILL', -1, NULL, '/^[0]*0$/');
    test_type($db, 230, 'NUMERIC ZEROFILL', 1, NULL, '/^[0]*1$/');
    test_type($db, 240, 'NUMERIC UNSIGNED ZEROFILL', 1.01, NULL, '/^[0]*1$/');

    echo "done!\n";
?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
done!
