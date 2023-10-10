--TEST--
PDO Common: Bug #35671 (binding by name breakage)
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

$db->exec('CREATE TABLE test35671 (field1 VARCHAR(32), field2 VARCHAR(32), field3 VARCHAR(32))');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$insert = $db->prepare("insert into test35671 (field1, field2, field3) values (:value1, :value2, :value3)");

$parm = array(
    ":value1" => 15,
    ":value2" => 20,
    ":value3" => 25
);

$insert->execute($parm);
$insert = null;

var_dump($db->query("SELECT * from test35671")->fetchAll(PDO::FETCH_ASSOC));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test35671");
?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    ["field1"]=>
    string(2) "15"
    ["field2"]=>
    string(2) "20"
    ["field3"]=>
    string(2) "25"
  }
}
