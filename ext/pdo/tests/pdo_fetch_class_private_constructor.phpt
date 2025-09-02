--TEST--
PDO Common: PDO::FETCH_CLASS with private constructor and arg by value
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

$db->exec('CREATE TABLE pdo_fetch_class_private_ctor(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_private_ctor VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO pdo_fetch_class_private_ctor VALUES(2, 'B', 'BB')");
$stmt = $db->prepare('SELECT id, val FROM pdo_fetch_class_private_ctor');

class TestPrivateCtor
{
    public $id;
    public $val;

    private function __construct(string $str)
    {
        echo __METHOD__ . "($str, {$this->id})\n";
    }
}

class TestDerivedPrivateCtor extends TestPrivateCtor {}

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestPrivateCtor', ['test']));

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestDerivedPrivateCtor', ['testFromDerived']));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_private_ctor");
?>
--EXPECTF--
TestPrivateCtor::__construct(test, 1)
TestPrivateCtor::__construct(test, 2)
array(2) {
  [0]=>
  object(TestPrivateCtor)#%d (2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  object(TestPrivateCtor)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
}
TestPrivateCtor::__construct(testFromDerived, 1)
TestPrivateCtor::__construct(testFromDerived, 2)
array(2) {
  [0]=>
  object(TestDerivedPrivateCtor)#%d (2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  object(TestDerivedPrivateCtor)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
}
