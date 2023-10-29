--TEST--
MySQL PDO->exec(), affected rows
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

// Run test only locally - not against remote hosts
$db = MySQLPDOTest::factory();
$stmt = $db->query('SELECT USER() as _user');
$row = $stmt->fetch(PDO::FETCH_ASSOC);
$tmp = explode('@', $row['_user']);
if (count($tmp) < 2)
    die("skip Cannot detect if test is run against local or remote database server");
if (($tmp[1] !== 'localhost') && ($tmp[1] !== '127.0.0.1'))
    die("skip Test cannot be run against remote database server");

$stmt = $db->query("SHOW VARIABLES LIKE 'secure_file_priv'");
if (($row = $stmt->fetch(PDO::FETCH_ASSOC)) && ($row['value'] != '')) {
    if (!is_writable($row['value']))
        die("skip secure_file_priv directory not writable: {$row['value']}");

    $filename = $row['value'] . DIRECTORY_SEPARATOR  . "pdo_mysql_exec_load_data.csv";

    if (file_exists($filename) && !is_writable($filename))
        die("skip {$filename} not writable");
}

?>
--FILE--
<?php
    function exec_and_count($offset, &$db, $sql, $exp) {

        try {

            $ret = $db->exec($sql);
            if ($ret !== $exp) {
                printf("[%03d] Expecting '%s'/%s got '%s'/%s when running '%s', [%s] %s\n",
                    $offset, $exp, gettype($exp), $ret, gettype($ret), $sql,
                    $db->errorCode(), implode(' ', $db->errorInfo()));
                return false;
            }

        } catch (PDOException $e) {

            if (42000 == $db->errorCode()) {
                // Error: 1148 SQLSTATE: 42000  (ER_NOT_ALLOWED_COMMAND)
                // Load data infile not allowed
                return false;
            }

            printf("[%03d] '%s' has failed, [%s] %s\n",
                $offset, $sql, $db->errorCode(), implode(' ', $db->errorInfo()));
            return false;
        }

        return true;
    }

    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    putenv('PDOTEST_ATTR='.serialize([PDO::MYSQL_ATTR_LOCAL_INFILE=>true]));
    $db = MySQLPDOTest::factory();
    MySQLPDOTest::createTestTable($db, MySQLPDOTest::detect_transactional_mysql_engine($db));

    /* affected rows related */

    exec_and_count(2, $db, 'DROP TABLE IF EXISTS test', 0);
    exec_and_count(3, $db, sprintf('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, col1 CHAR(10)) ENGINE=%s', PDO_MYSQL_TEST_ENGINE), 0);

    $stmt = $db->query("SHOW VARIABLES LIKE 'secure_file_priv'");
    if (($row = $stmt->fetch(PDO::FETCH_ASSOC)) && ($row['value'] != '')) {
        $filename = $row['value'] . DIRECTORY_SEPARATOR  . "pdo_mysql_exec_load_data.csv";
    } else {
        $filename =  MySQLPDOTest::getTempDir() . DIRECTORY_SEPARATOR  . "pdo_mysql_exec_load_data.csv";
    }

    $fp = fopen($filename, "w");
    fwrite($fp, "1;foo\n");
    fwrite($fp, "2;bar");
    fclose($fp);

    $sql = sprintf("LOAD DATA LOCAL INFILE %s INTO TABLE test FIELDS TERMINATED BY ';' LINES TERMINATED  BY '\n'", $db->quote($filename));

    if (exec_and_count(4, $db, $sql, 2)) {

        $stmt = $db->query('SELECT id, col1 FROM test ORDER BY id ASC');
        $expected = array(array("id" => 1, "col1" => "foo"), array("id" => 2, "col1" => "bar"));
        $ret = $stmt->fetchAll(PDO::FETCH_ASSOC);
        foreach ($expected as $offset => $exp) {
            foreach ($exp as $key => $value) {
                if ($ret[$offset][$key] != $value) {
                    printf("Results seem wrong, check manually\n");
                    var_dump($ret);
                    var_dump($expected);
                    break 2;
                }
            }
        }
    }

    unlink($filename);

    print "done!";
?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'inc' . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
done!
