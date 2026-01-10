--TEST--
PDO Common: PDO::query() with PDO::FETCH_LAZY unused result
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

$db->exec('create table pdo_query_fetch_lazy_001 (id int, name varchar(10))');
$db->exec("INSERT INTO pdo_query_fetch_lazy_001 (id,name) VALUES(1,'test1')");
$db->exec("INSERT INTO pdo_query_fetch_lazy_001 (id,name) VALUES(2,'test2')");

$r = $db->query("SELECT * FROM pdo_query_fetch_lazy_001", PDO::FETCH_LAZY);
var_dump($r);
echo "End\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_query_fetch_lazy_001");
?>
--EXPECT--
object(PDOStatement)#2 (1) {
  ["queryString"]=>
  string(38) "SELECT * FROM pdo_query_fetch_lazy_001"
}
End
