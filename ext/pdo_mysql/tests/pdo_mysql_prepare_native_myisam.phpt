--TEST--
MySQL PDO->prepare(), native PS
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    function prepex($offset, &$db, $query, $input_params = null, $error_info = null, $suppress_warning = false) {

        try {

            if ($suppress_warning || (is_array($error_info) && isset($error_info['prepare'])))
                $stmt = @$db->prepare($query);
            else
                $stmt = $db->prepare($query);

            if (is_array($error_info) && isset($error_info['prepare'])) {
                $tmp = $db->errorInfo();

                if (isset($error_info['prepare']['sqlstate']) &&
                    ($error_info['prepare']['sqlstate'] !== $tmp[0])) {
                    printf("[%03d] prepare() - expecting SQLSTATE '%s' got '%s'\n",
                        $offset, $error_info['prepare']['sqlstate'], $tmp[0]);
                    return false;
                }

                if (isset($error_info['prepare']['mysql']) &&
                    ($error_info['prepare']['mysql'] !== $tmp[1])) {
                    printf("[%03d] prepare() - expecting MySQL Code '%s' got '%s'\n",
                        $offset, $error_info['prepare']['mysql'], $tmp[0]);
                    return false;
                }

                return false;
            }

            if (!is_object($stmt))
                return false;

            if (is_null($input_params))
                $input_params = array();
// 5.0.18, 5.1.14 @ 15
// printf("[%03d]\n", $offset);
            if ($suppress_warning || (is_array($error_info) && isset($error_info['execute'])))
                $ret = @$stmt->execute($input_params);
            else
                $ret = $stmt->execute($input_params);

            if (!is_bool($ret))
                printf("[%03d] PDO::execute() should return a boolean value, got %s/%s\n",
                    var_export($ret, true), $ret);

            $tmp = $stmt->errorInfo();
            if (isset($tmp[1]) && ($tmp[1] == 2030)) {
                // Trying to hack around MySQL Server version dependent features
                // 2030 This command is not supported in the prepared statement protocol yet
                return false;
            }

            if (is_array($error_info) && isset($error_info['execute'])) {

                if (isset($error_info['execute']['sqlstate']) &&
                    ($error_info['execute']['sqlstate'] !== $tmp[0])) {
                    printf("[%03d] execute() - expecting SQLSTATE '%s' got '%s'\n",
                        $offset, $error_info['execute']['sqlstate'], $tmp[0]);
                    return false;
                }

                if (isset($error_info['execute']['mysql']) &&
                    ($error_info['execute']['mysql'] !== $tmp[1])) {
                    printf("[%03d] execute() - expecting MySQL Code '%s' got '%s'\n",
                        $offset, $error_info['execute']['mysql'], $tmp[0]);
                    return false;
                }

                return false;

            }

        } catch (PDOException $e) {
            printf("[%03d] %s, [%s} %s\n",
                $offset, $e->getMessage(),
                $db->errorCode(), implode(' ', $db->errorInfo()));
            return false;
        }

        return $stmt;
    }

    try {
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
        if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to turn off emulated prepared statements\n");

        try {
            prepex(3, $db, '', [], ['prepare' => ['sqlstate' => '42000']]);
        } catch (\ValueError $e) {
            echo $e->getMessage(), \PHP_EOL;
        }

        prepex(4, $db, 'CREATE TABLE test_prepare_native_myisam(id INT, label CHAR(255)) ENGINE=MyISAM');
        // Not every MySQL Server version supports this
        if (is_object(prepex(5, $db, 'CREATE FULLTEXT INDEX idx1 ON test_prepare_native_myisam(label)', null, null, true))) {
            prepex(6, $db, 'INSERT INTO test_prepare_native_myisam(id, label) VALUES (1, :placeholder)',
                array(':placeholder' => 'MySQL is the best database in the world!'));
            prepex(7, $db, 'INSERT INTO test_prepare_native_myisam(id, label) VALUES (2, :placeholder)',
                array(':placeholder' => 'If I have the freedom to choose, I would always go again for the MySQL Server'));
            $stmt = prepex(8, $db, 'SELECT id, label FROM test_prepare_native_myisam WHERE MATCH label AGAINST (:placeholder)',
                array(':placeholder' => 'mysql'), null, true);
            if (is_object($stmt)) {
                /*
                Lets ignore this
                if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
                    printf("[033] Expecting two rows, got %d rows\n", $tmp);
                */
                $stmt = prepex(9, $db, 'SELECT id, label FROM test_prepare_native_myisam ORDER BY id ASC');
                if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
                    printf("[027] Expecting two rows, got %d rows\n", $tmp);

                if ($tmp[0]['label'] !== 'MySQL is the best database in the world!') {
                    printf("[028] INSERT seems to have failed, dumping data, check manually\n");
                    var_dump($tmp);
                }
            }
        }

        $db->exec('DELETE FROM test_prepare_native_myisam');
        $db->exec("INSERT INTO test_prepare_native_myisam(id, label) VALUES (1, 'row1'), (2, 'row2')");

        $sql = sprintf("SELECT id, label FROM test_prepare_native_myisam WHERE (label LIKE %s) AND (id = :placeholder)",
            $db->quote('%ro%'));
        $stmt = prepex(10, $db, $sql,	array('placeholder' => -1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
                printf("[030] Expecting zero rows, got %d rows\n", $tmp);

        $sql = sprintf("SELECT id, label FROM test_prepare_native_myisam WHERE  (id = :placeholder) OR (label LIKE %s)",
            $db->quote('%go%'));
        $stmt = prepex(11, $db, $sql,	array('placeholder' => 1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 1)
            printf("[032] Expecting one row, got %d rows\n", $tmp);
    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable(NULL, 'test_prepare_native_myisam');
?>
--EXPECT--
PDO::prepare(): Argument #1 ($query) cannot be empty
done!
