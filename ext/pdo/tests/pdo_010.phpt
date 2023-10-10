--TEST--
PDO Common: PDO::FETCH_CLASSTYPE and GROUP/UNIQUE
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

$db->exec('CREATE TABLE classtypes010(id int NOT NULL PRIMARY KEY, name VARCHAR(10) NOT NULL UNIQUE)');
$db->exec('INSERT INTO classtypes010 VALUES(0, \'stdClass\')');
$db->exec('INSERT INTO classtypes010 VALUES(1, \'Test1\')');
$db->exec('INSERT INTO classtypes010 VALUES(2, \'Test2\')');
$db->exec('CREATE TABLE test010(id int NOT NULL PRIMARY KEY, classtype int, val VARCHAR(10), grp VARCHAR(10))');
$db->exec('INSERT INTO test010 VALUES(1, 0, \'A\', \'Group1\')');
$db->exec('INSERT INTO test010 VALUES(2, 1, \'B\', \'Group1\')');
$db->exec('INSERT INTO test010 VALUES(3, 2, \'C\', \'Group2\')');
$db->exec('INSERT INTO test010 VALUES(4, 3, \'D\', \'Group2\')');

$stmt = $db->prepare('SELECT classtypes.name, test.grp AS grp, test.id AS id, test.val AS val FROM test010 LEFT JOIN classtypes010 ON test.classtype=classtypes.id');

class Test1
{
    public $id;
    public $val;

    public function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}

class Test2
{
    public $id;
    public $val;

    public function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}

class Test3
{
    public $id;
    public $val;

    public function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}


$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_CLASSTYPE|PDO::FETCH_GROUP, 'Test3'));

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_CLASSTYPE|PDO::FETCH_UNIQUE, 'Test3'));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test010");
$db->exec("DROP TABLE classtypes010");
?>
--EXPECTF--
Test1::__construct()
Test2::__construct()
Test3::__construct()
array(2) {
  ["Group1"]=>
  array(2) {
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
  }
  ["Group2"]=>
  array(2) {
    [0]=>
    object(Test2)#%d (2) {
      ["id"]=>
      string(1) "3"
      ["val"]=>
      string(1) "C"
    }
    [1]=>
    object(Test3)#%d (2) {
      ["id"]=>
      string(1) "4"
      ["val"]=>
      string(1) "D"
    }
  }
}
Test1::__construct()
Test2::__construct()
Test3::__construct()
array(2) {
  ["Group1"]=>
  object(Test1)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  ["Group2"]=>
  object(Test3)#%d (2) {
    ["id"]=>
    string(1) "4"
    ["val"]=>
    string(1) "D"
  }
}
