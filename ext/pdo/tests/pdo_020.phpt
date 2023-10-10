--TEST--
PDO Common: PDOStatement::columnCount
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test020(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');
$db->exec("INSERT INTO test020 VALUES(1, 'A', 'A')");
$db->exec("INSERT INTO test020 VALUES(2, 'B', 'B')");
$db->exec("INSERT INTO test020 VALUES(3, 'C', 'C')");

foreach (array('SELECT id, val FROM test020', 'SELECT id, val, val2 FROM test020', 'SELECT COUNT(*) FROM test020') as $sql) {

    $stmt = $db->query($sql);
    $res = $stmt->columnCount();
    echo "Counted $res columns after $sql.\n";
    $stmt = null;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test020");
?>
--EXPECT--
Counted 2 columns after SELECT id, val FROM test020.
Counted 3 columns after SELECT id, val, val2 FROM test020.
Counted 1 columns after SELECT COUNT(*) FROM test020.
