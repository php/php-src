--TEST--
PDO Common: PDO::FETCH_CLASS
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/'); 
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A', 'AA')");
$db->exec("INSERT INTO test VALUES(2, 'B', 'BB')");
$db->exec("INSERT INTO test VALUES(3, 'C', 'CC')");

$stmt = $db->prepare('SELECT id, val, val2 from test');

class TestBase
{
	public $id;
	protected $val;
	private $val2;
}

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
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (3) {
    [u"id"]=>
    unicode(1) "1"
    [u"val"]=>
    unicode(1) "A"
    [u"val2"]=>
    unicode(2) "AA"
  }
  [1]=>
  object(stdClass)#%d (3) {
    [u"id"]=>
    unicode(1) "2"
    [u"val"]=>
    unicode(1) "B"
    [u"val2"]=>
    unicode(2) "BB"
  }
  [2]=>
  object(stdClass)#%d (3) {
    [u"id"]=>
    unicode(1) "3"
    [u"val"]=>
    unicode(1) "C"
    [u"val2"]=>
    unicode(2) "CC"
  }
}
array(3) {
  [0]=>
  object(TestBase)#%d (3) {
    [u"id"]=>
    unicode(1) "1"
    [u"val":protected]=>
    unicode(1) "A"
    [u"val2":u"TestBase":private]=>
    unicode(2) "AA"
  }
  [1]=>
  object(TestBase)#%d (3) {
    [u"id"]=>
    unicode(1) "2"
    [u"val":protected]=>
    unicode(1) "B"
    [u"val2":u"TestBase":private]=>
    unicode(2) "BB"
  }
  [2]=>
  object(TestBase)#%d (3) {
    [u"id"]=>
    unicode(1) "3"
    [u"val":protected]=>
    unicode(1) "C"
    [u"val2":u"TestBase":private]=>
    unicode(2) "CC"
  }
}
TestDerived::__construct(0,1)
TestDerived::__construct(1,2)
TestDerived::__construct(2,3)
array(3) {
  [0]=>
  object(TestDerived)#%d (5) {
    [u"row":protected]=>
    int(0)
    [u"id"]=>
    unicode(1) "1"
    [u"val":protected]=>
    unicode(1) "A"
    [u"val2":u"TestBase":private]=>
    NULL
    [u"val2"]=>
    unicode(2) "AA"
  }
  [1]=>
  object(TestDerived)#%d (5) {
    [u"row":protected]=>
    int(1)
    [u"id"]=>
    unicode(1) "2"
    [u"val":protected]=>
    unicode(1) "B"
    [u"val2":u"TestBase":private]=>
    NULL
    [u"val2"]=>
    unicode(2) "BB"
  }
  [2]=>
  object(TestDerived)#%d (5) {
    [u"row":protected]=>
    int(2)
    [u"id"]=>
    unicode(1) "3"
    [u"val":protected]=>
    unicode(1) "C"
    [u"val2":u"TestBase":private]=>
    NULL
    [u"val2"]=>
    unicode(2) "CC"
  }
}
