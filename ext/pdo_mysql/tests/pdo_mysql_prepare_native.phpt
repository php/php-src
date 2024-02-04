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

        // lets be fair and do the most simple SELECT first
        $stmt = prepex(4, $db, 'SELECT 1 as "one"');
        if (MySQLPDOTest::isPDOMySQLnd())
            // native types - int
            $expected = array('one' => 1);
        else
            // always strings, like STRINGIFY flag
            $expected = array('one' => '1');

        $row = $stmt->fetch(PDO::FETCH_ASSOC);
        if ($row !== $expected) {
            printf("[004a] Expecting %s got %s\n", var_export($expected, true), var_export($row, true));
        }

        prepex(6, $db, sprintf('CREATE TABLE test_prepare_native(id INT, label CHAR(255)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE));
        prepex(7, $db, "INSERT INTO test_prepare_native(id, label) VALUES(1, ':placeholder')");
        $stmt = prepex(8, $db, 'SELECT label FROM test_prepare_native ORDER BY id ASC');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        prepex(9, $db, 'DELETE FROM test_prepare_native');
        prepex(10, $db, 'INSERT INTO test_prepare_native(id, label) VALUES(1, :placeholder)',
            array(':placeholder' => 'first row'));
        prepex(11, $db, 'INSERT INTO test_prepare_native(id, label) VALUES(2, :placeholder)',
            array(':placeholder' => 'second row'));
        $stmt = prepex(12, $db, 'SELECT label FROM test_prepare_native ORDER BY id ASC');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        // Is PDO fun?
        $stmt = prepex(13, $db, 'SELECT label FROM test_prepare_native WHERE :placeholder > 1',
            array(':placeholder' => 'id'));
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

        for ($num_params = 2; $num_params < 100; $num_params++) {
            $params = array(':placeholder' => 'a');
            for ($i = 1; $i < $num_params; $i++) {
                $params[str_repeat('a', $i)] = 'some data';
            }
            prepex(16, $db, 'SELECT id, label FROM test_prepare_native WHERE label > :placeholder',
                $params, array('execute' => array('sqlstate' => 'HY093')));
        }

        $stmt = prepex(16, $db, 'SELECT id, label FROM test_prepare_native WHERE :placeholder IS NOT NULL',
            array(':placeholder' => 1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 2)
            printf("[017] '1' IS NOT NULL evaluates to true, expecting two rows, got %d rows\n", $tmp);

        $stmt = prepex(18, $db, 'SELECT id, label FROM test_prepare_native WHERE :placeholder IS NULL',
            array(':placeholder' => 1));
        if (count(($tmp = $stmt->fetchAll(PDO::FETCH_ASSOC))) != 0)
            printf("[019] '1' IS NOT NULL evaluates to true, expecting zero rows, got %d rows\n", $tmp);
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
$db->exec('DROP TABLE IF EXISTS test_prepare_native');
?>
--EXPECT--
PDO::prepare(): Argument #1 ($query) cannot be empty
array(1) {
  [0]=>
  array(1) {
    ["label"]=>
    string(12) ":placeholder"
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
array(0) {
}
done!
