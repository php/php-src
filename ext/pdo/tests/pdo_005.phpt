--TEST--
PDO Common: PDO::FETCH_CLASS
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

$db->exec('CREATE TABLE test005(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO test005 VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO test005 VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO test005 VALUES(3, 'C', 'CC')");

$stmt = $db->prepare('SELECT id, val, val2 from test');

class TestBase
{
    public $id;
    protected $val;
    private $val2;
}

#[AllowDynamicProperties] // $val2 will be dynamic now.
class TestDerived extends TestBase
{
    protected $row;

    public function __construct(&$row)
    {
        echo __METHOD__ . "($row,{$this->id})\n";
        $this->row = $row++;
    }
}

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS));

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestBase'));

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS, 'TestDerived', array(0)));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test005");
?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (3) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
    ["val2"]=>
    string(2) "AA"
  }
  [1]=>
  object(stdClass)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
    ["val2"]=>
    string(2) "BB"
  }
  [2]=>
  object(stdClass)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
    ["val2"]=>
    string(2) "CC"
  }
}
array(3) {
  [0]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "1"
    ["val":protected]=>
    string(1) "A"
    ["val2":"TestBase":private]=>
    string(2) "AA"
  }
  [1]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val":protected]=>
    string(1) "B"
    ["val2":"TestBase":private]=>
    string(2) "BB"
  }
  [2]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val":protected]=>
    string(1) "C"
    ["val2":"TestBase":private]=>
    string(2) "CC"
  }
}
TestDerived::__construct(0,1)
TestDerived::__construct(1,2)
TestDerived::__construct(2,3)
array(3) {
  [0]=>
  object(TestDerived)#%d (5) {
    ["id"]=>
    string(1) "1"
    ["val":protected]=>
    string(1) "A"
    ["val2":"TestBase":private]=>
    NULL
    ["row":protected]=>
    int(0)
    ["val2"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestDerived)#%d (5) {
    ["id"]=>
    string(1) "2"
    ["val":protected]=>
    string(1) "B"
    ["val2":"TestBase":private]=>
    NULL
    ["row":protected]=>
    int(1)
    ["val2"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestDerived)#%d (5) {
    ["id"]=>
    string(1) "3"
    ["val":protected]=>
    string(1) "C"
    ["val2":"TestBase":private]=>
    NULL
    ["row":protected]=>
    int(2)
    ["val2"]=>
    string(2) "CC"
  }
}
