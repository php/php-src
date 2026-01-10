--TEST--
PDO Common: fetch() and while()
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

$db->exec('CREATE TABLE test019(idx int NOT NULL PRIMARY KEY, txt VARCHAR(20))');
$db->exec("INSERT INTO test019 VALUES(0, 'String0')");
$db->exec("INSERT INTO test019 VALUES(1, 'String1')");
$db->exec("INSERT INTO test019 VALUES(2, 'String2')");
$db->exec("INSERT INTO test019 VALUES(3, 'String3')");


var_dump($db->query('SELECT COUNT(*) FROM test019')->fetchColumn());

$stmt = $db->prepare('SELECT idx, txt FROM test019 ORDER by idx');

$stmt->execute();
$cont = $stmt->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE);
var_dump($cont);

echo "===WHILE===\n";

$stmt->bindColumn('idx', $idx);
$stmt->bindColumn('txt', $txt);
$stmt->execute();

while($stmt->fetch(PDO::FETCH_BOUND)) {
    var_dump(array($idx=>$txt));
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test019");
?>
--EXPECT--
string(1) "4"
array(4) {
  [0]=>
  string(7) "String0"
  [1]=>
  string(7) "String1"
  [2]=>
  string(7) "String2"
  [3]=>
  string(7) "String3"
}
===WHILE===
array(1) {
  [0]=>
  string(7) "String0"
}
array(1) {
  [1]=>
  string(7) "String1"
}
array(1) {
  [2]=>
  string(7) "String2"
}
array(1) {
  [3]=>
  string(7) "String3"
}
