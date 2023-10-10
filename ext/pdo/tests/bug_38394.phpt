--TEST--
PDO Common: Bug #38394 (Prepared statement error stops subsequent statements)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'sqlite2', strlen('sqlite2'))) die('skip not relevant for pdo_sqlite2 driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();
$db->exec("CREATE TABLE test38394 (a INT, b INT, c INT)");
$s = $db->prepare("INSERT INTO test38394 (a,b,c) VALUES (:a,:b,:c)");

$s->execute(array('a' => 1, 'b' => 2, 'c' => 3));

@$s->execute(array('a' => 5, 'b' => 6, 'c' => 7, 'd' => 8));

$s->execute(array('a' => 9, 'b' => 10, 'c' => 11));

var_dump($db->query("SELECT * FROM test38394")->fetchAll(PDO::FETCH_ASSOC));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test38394");
?>
--EXPECT--
array(2) {
  [0]=>
  array(3) {
    ["a"]=>
    string(1) "1"
    ["b"]=>
    string(1) "2"
    ["c"]=>
    string(1) "3"
  }
  [1]=>
  array(3) {
    ["a"]=>
    string(1) "9"
    ["b"]=>
    string(2) "10"
    ["c"]=>
    string(2) "11"
  }
}
