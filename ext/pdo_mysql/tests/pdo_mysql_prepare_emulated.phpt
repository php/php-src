--TEST--
MySQL PDO->prepare(), emulated PS
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
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    function prepex($offset, &$db, $query, $input_params = null, $error_info = null) {
        try {
            if (is_array($error_info) && isset($error_info['prepare']))
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

            if (is_null($input_params))
                $input_params = array();

            if (is_array($error_info) && isset($error_info['execute']))
                $ret = @$stmt->execute($input_params);
            else
                $ret = $stmt->execute($input_params);

            if (!is_bool($ret))
                printf("[%03d] PDO::execute() should return a boolean value, got %s/%s\n",
                    var_export($ret, true), $ret);

            if (is_array($error_info) && isset($error_info['execute'])) {
                $tmp = $stmt->errorInfo();

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
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
        if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to switch to emulated prepared statements, test will fail\n");

        try {
            prepex(3, $db, '', [], ['execute' => ['sqlstate' => '42000']]);
        } catch (\ValueError $e) {
            echo $e->getMessage(), \PHP_EOL;
        }

        // lets be fair and do the most simple SELECT first
        $stmt = prepex(4, $db, 'SELECT 1 AS "one"');
        var_dump($stmt->fetch(PDO::FETCH_ASSOC));

        prepex(6, $db, sprintf('CREATE TABLE test_prepare_emulated(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
        prepex(7, $db, "INSERT INTO test_prepare_emulated(id, label) VALUES(1, ':placeholder')");
        $stmt = prepex(8, $db, 'SELECT label FROM test_prepare_emulated');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        prepex(9, $db, 'DELETE FROM test_prepare_emulated');
        prepex(10, $db, "INSERT INTO test_prepare_emulated(id, label) VALUES(1, ':placeholder')",
            array(':placeholder' => 'first row'));
        $stmt = prepex(11, $db, 'SELECT label FROM test_prepare_emulated');

        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        prepex(12, $db, 'DELETE FROM test_prepare_emulated');
        prepex(13, $db, 'INSERT INTO test_prepare_emulated(id, label) VALUES(1, :placeholder)',
            array(':placeholder' => 'first row'));
        prepex(14, $db, 'INSERT INTO test_prepare_emulated(id, label) VALUES(2, :placeholder)',
            array(':placeholder' => 'second row'));
        $stmt = prepex(15, $db, 'SELECT label FROM test_prepare_emulated');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        // Is PDO fun?
        prepex(16, $db, 'SELECT label FROM test_prepare_emulated WHERE :placeholder > 1',
            array(':placeholder' => 'id'));
        prepex(17, $db, 'SELECT :placeholder FROM test_prepare_emulated WHERE id > 1',
            array(':placeholder' => 'id'));
        prepex(18, $db, 'SELECT :placeholder FROM test_prepare_emulated WHERE :placeholder > :placeholder',
            array(':placeholder' => 'test'));

        for ($num_params = 2; $num_params < 100; $num_params++) {
            $params = array(':placeholder' => 'a');
            for ($i = 1; $i < $num_params; $i++) {
                $params[str_repeat('a', $i)] = 'some data';
            }
            prepex(19, $db, 'SELECT id, label FROM test_prepare_emulated WHERE label > :placeholder',
                $params, array('execute' => array('sqlstate' => 'HY093')));
        }

        prepex(20, $db, 'DELETE FROM test_prepare_emulated');
        prepex(21, $db, 'INSERT INTO test_prepare_emulated(id, label) VALUES (1, :placeholder), (2, :placeholder)',
            array(':placeholder' => 'row'));
        $stmt = prepex(22, $db, 'SELECT id, label FROM test_prepare_emulated');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        $stmt = prepex(23, $db, 'SELECT id, label FROM test_prepare_emulated WHERE :placeholder IS NOT NULL',
            array(':placeholder' => 1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
            printf("[024] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

        $stmt = prepex(25, $db, 'SELECT id, label FROM test_prepare_emulated WHERE :placeholder IS NULL',
            array(':placeholder' => 1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
            printf("[026] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);

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
$db->exec('DROP TABLE IF EXISTS test_prepare_emulated');
?>
--EXPECTF--
PDO::prepare(): Argument #1 ($query) cannot be empty
array(1) {
  ["one"]=>
  string(1) "1"
}
array(1) {
  [0]=>
  array(1) {
    ["label"]=>
    string(12) ":placeholder"
  }
}

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
array(0) {
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
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(3) "row"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(3) "row"
  }
}
done!
