--TEST--
PDO Common: Bug #42917 (PDO::FETCH_KEY_PAIR doesn't work with setFetchMode)
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

$db->exec("CREATE TABLE test42917 (a varchar(100), b varchar(100), c varchar(100))");

for ($i = 0; $i < 5; $i++) {
    $db->exec("INSERT INTO test42917 (a,b,c) VALUES('test".$i."','".$i."','".$i."')");
}

$res = $db->query("SELECT a,b FROM test42917");
$res->setFetchMode(PDO::FETCH_KEY_PAIR);
var_dump($res->fetchAll());

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test42917");
?>
--EXPECT--
array(5) {
  ["test0"]=>
  string(1) "0"
  ["test1"]=>
  string(1) "1"
  ["test2"]=>
  string(1) "2"
  ["test3"]=>
  string(1) "3"
  ["test4"]=>
  string(1) "4"
}
