--TEST--
PDO Common: PECL Bug #5772 (PDO::FETCH_FUNC breaks on mixed case func name)
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

$db->exec("CREATE TABLE test5772 (id int NOT NULL, PRIMARY KEY (id))");
$db->exec("INSERT INTO test5772 (id) VALUES (1)");

function heLLO($row) {
    return $row;
}

foreach ($db->query("SELECT * FROM test5772")->fetchAll(PDO::FETCH_FUNC, 'heLLO') as $row) {
    var_dump($row);
}
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test5772");
?>
--EXPECT--
string(1) "1"
