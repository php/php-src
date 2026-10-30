--TEST--
PDO Common: PDO::FETCH_LAZY
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

$db->exec('create table test027 (id int, name varchar(10))');
$db->exec("INSERT INTO test027 (id,name) VALUES(1,'test1')");
$db->exec("INSERT INTO test027 (id,name) VALUES(2,'test2')");

foreach ($db->query("SELECT * FROM test027", PDO::FETCH_LAZY) as $v) {
    echo "lazy: " . $v->id.$v->name."\n";
}
echo "End\n";
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test027");
?>
--EXPECT--
lazy: 1test1
lazy: 2test2
End
