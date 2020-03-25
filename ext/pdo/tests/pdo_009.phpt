--TEST--
PDO Common: PDO::FETCH_CLASSTYPE
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
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

$db->exec('CREATE TABLE classtypes(id int NOT NULL PRIMARY KEY, name VARCHAR(10) NOT NULL UNIQUE)');
$db->exec('INSERT INTO classtypes VALUES(0, \'stdClass\')');
$db->exec('INSERT INTO classtypes VALUES(1, \'Test1\')');
$db->exec('INSERT INTO classtypes VALUES(2, \'Test2\')');
$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, classtype int, val VARCHAR(10))');
$db->exec('INSERT INTO test VALUES(1, 0, \'A\')');
$db->exec('INSERT INTO test VALUES(2, 1, \'B\')');
$db->exec('INSERT INTO test VALUES(3, 2, \'C\')');
$db->exec('INSERT INTO test VALUES(4, 3, \'D\')');

$stmt = $db->prepare('SELECT classtypes.name, test.id AS id, test.val AS val FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id');

class Test1
{
    public function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}

class Test2
{
    public function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}

class Test3
{
    public function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_NUM));

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_CLASSTYPE, 'Test3'));

?>
--EXPECTF--
array(4) {
  [0]=>
  array(3) {
    [0]=>
    string(8) "stdClass"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "A"
  }
  [1]=>
  array(3) {
    [0]=>
    string(5) "Test1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "B"
  }
  [2]=>
  array(3) {
    [0]=>
    string(5) "Test2"
    [1]=>
    string(1) "3"
    [2]=>
    string(1) "C"
  }
  [3]=>
  array(3) {
    [0]=>
    NULL
    [1]=>
    string(1) "4"
    [2]=>
    string(1) "D"
  }
}
Test1::__construct()
Test2::__construct()
Test3::__construct()
array(4) {
  [0]=>
  object(stdClass)#%d (2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  [2]=>
  object(Test2)#%d (2) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
  [3]=>
  object(Test3)#%d (2) {
    ["id"]=>
    string(1) "4"
    ["val"]=>
    string(1) "D"
  }
}
