--TEST--
MySQL PDO->prepare(), native PS
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

        prepex(4, $db, sprintf('CREATE TABLE test_prepare_native_anonymous_placeholder(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
        prepex(5, $db, "INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES(1, '?')");
        $stmt = prepex(6, $db, 'SELECT label FROM test_prepare_native_anonymous_placeholder');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        prepex(7, $db, 'DELETE FROM test_prepare_native_anonymous_placeholder');
        prepex(8, $db, 'INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES(1, ?)',
            array('first row'));
        prepex(9, $db, 'INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES(2, ?)',
            array('second row'));
        $stmt = prepex(10, $db, 'SELECT label FROM test_prepare_native_anonymous_placeholder');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        // Is PDO fun?
        prepex(10, $db, 'SELECT label FROM test_prepare_native_anonymous_placeholder WHERE ? > 1',
            array('id'));
        prepex(11, $db, 'SELECT ? FROM test_prepare_native_anonymous_placeholder WHERE id > 1',
            array('id'));
        prepex(12, $db, 'SELECT ? FROM test_prepare_native_anonymous_placeholder WHERE ? > ?',
            array('id', 'label', 'value'));

        for ($num_params = 2; $num_params < 100; $num_params++) {
            $params = array('a');
            for ($i = 1; $i < $num_params; $i++) {
                $params[] = 'some data';
            }
            prepex(13, $db, 'SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE label > ?',
                $params, array('execute' => array('sqlstate' => 'HY093')));
        }

        prepex(14, $db, 'DELETE FROM test_prepare_native_anonymous_placeholder');
        prepex(15, $db, 'INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES (1, ?), (2, ?)',
            array('row', 'row'));
        $stmt = prepex(16, $db, 'SELECT id, label FROM test_prepare_native_anonymous_placeholder ORDER BY id');
        $tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
        $exp = array(
            0 => array(
                "id"  => "1",
                "label" => "row"
            ),
            1 => array(
                "id" => "2",
                "label" => "row"
            ),
        );

        if (MySQLPDOTest::isPDOMySQLnd()) {
            // mysqlnd returns native types
            $exp[0]['id'] = 1;
            $exp[1]['id'] = 2;
        }
        if ($tmp !== $exp) {
            printf("[064] Results seem wrong. Please check dumps manually.\n");
            var_dump($exp);
            var_dump($tmp);
        }

        $stmt = prepex(17, $db, 'SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE ? IS NOT NULL',
            array(1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
            printf("[048] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

        $stmt = prepex(19, $db, 'SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE ? IS NULL',
            array(1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
            printf("[050] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);

        prepex(21, $db, 'DROP TABLE IF EXISTS test_prepare_native_anonymous_placeholder');
        prepex(22, $db, 'CREATE TABLE test_prepare_native_anonymous_placeholder(id INT, label CHAR(255)) ENGINE=MyISAM');
        if (is_object(prepex(23, $db, 'CREATE FULLTEXT INDEX idx1 ON test_prepare_native_anonymous_placeholder(label)', null, null, true))) {
            prepex(24, $db, 'INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES (1, ?)',
                array('MySQL is the best database in the world!'));
            prepex(25, $db, 'INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES (1, ?)',
                array('If I have the freedom to choose, I would always go again for the MySQL Server'));
            $stmt = prepex(26, $db, 'SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE MATCH label AGAINST (?)',
                array('mysql'), null, true);
            /*
            Lets ignore that
            if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
                printf("[074] Expecting two rows, got %d rows\n", $tmp);
            */
        }

        prepex(27, $db, 'DELETE FROM test_prepare_native_anonymous_placeholder');
        prepex(28, $db, 'INSERT INTO test_prepare_native_anonymous_placeholder(id, label) VALUES (1, ?), (2, ?)',
            array('row1', 'row2'));

        /*
        TODO enable after fix
        $stmt = prepex(27, $db, 'SELECT id, label FROM \'test_prepare_native_anonymous_placeholder WHERE MATCH label AGAINST (:placeholder)',
            array(':placeholder' => 'row'),
            array('execute' => array('sqlstate' => '42000', 'mysql' => 1064)));
        */

        $stmt = prepex(29, $db, 'SELECT id, label AS "label" FROM test_prepare_native_anonymous_placeholder WHERE label = ?',
            array('row1'));
        $tmp = $stmt->fetchAll(PDO::FETCH_ASSOC);
        $exp = array(
            0 => array("id" => "1", "label" => "row1")
        );

        if (MySQLPDOTest::isPDOMySQLnd()) {
            // mysqlnd returns native types
            $exp[0]['id'] = 1;
        }
        if ($tmp !== $exp) {
            printf("[065] Results seem wrong. Please check dumps manually.\n");
            var_dump($exp);
            var_dump($tmp);
        }

        $sql = sprintf("SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE (label LIKE %s) AND (id = ?)",
            $db->quote('%ro%'));
        $stmt = prepex(30, $db, $sql,	array(-1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
                printf("[061] Expecting zero rows, got %d rows\n", $tmp);

        $sql = sprintf("SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE  (id = ?) OR (label LIKE %s)",
            $db->quote('%ro%'));
        $stmt = prepex(31, $db, $sql,	array(1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
                printf("[062] Expecting two rows, got %d rows\n", $tmp);

        $sql = "SELECT id, label FROM test_prepare_native_anonymous_placeholder WHERE id = ? AND label = (SELECT label AS 'SELECT' FROM test_prepare_native_anonymous_placeholder WHERE id = ?)";
        $stmt = prepex(33, $db, $sql,	array(1, 1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 1)
                printf("[064] Expecting one row, got %d rows\n", $tmp);

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
$db->exec('DROP TABLE IF EXISTS test_prepare_native_anonymous_placeholder');
?>
--EXPECT--
PDO::prepare(): Argument #1 ($query) cannot be empty
array(1) {
  [0]=>
  array(1) {
    ["label"]=>
    string(1) "?"
  }
}
array(2) {
  [0]=>
  array(1) {
    ["label"]=>
    string(9) "first row"
  }
  [1]=>
  array(1) {
    ["label"]=>
    string(10) "second row"
  }
}
done!
