--TEST--
PDO-SQLite: PDO_FETCH_CLASS
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("pdo_sqlite")) print "skip"; ?>
--FILE--
<?php

$db =new pdo('sqlite::memory:');

$db->exec('CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
$db->exec('INSERT INTO test VALUES(1, "A", "AA")'); 
$db->exec('INSERT INTO test VALUES(2, "B", "BB")'); 
$db->exec('INSERT INTO test VALUES(3, "C", "CC")'); 

class TestBase
{
	public $id;
	protected $val;
	private $val2;
}

class TestDerived extends TestBase
{
	protected $p1;
	protected $p2;

	public function __construct($p1, $p2)
	{
		$this->p1 = $p1;
		$this->p2 = $p2;
	}
}

var_dump($db->query('SELECT * FROM test')->fetchAll(PDO_FETCH_CLASS));
var_dump($db->query('SELECT * FROM test')->fetchAll(PDO_FETCH_CLASS, 'TestBase'));
var_dump($db->query('SELECT * FROM test')->fetchAll(PDO_FETCH_CLASS, 'TestDerived', array(1,2)));
?>
===DONE===
<?php exit(0); ?>
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
    ["val:protected"]=>
    string(1) "A"
    ["val2:private"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "2"
    ["val:protected"]=>
    string(1) "B"
    ["val2:private"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestBase)#%d (3) {
    ["id"]=>
    string(1) "3"
    ["val:protected"]=>
    string(1) "C"
    ["val2:private"]=>
    string(2) "CC"
  }
}
array(3) {
  [0]=>
  object(TestDerived)#%d (6) {
    ["p1:protected"]=>
    int(1)
    ["p2:protected"]=>
    int(2)
    ["id"]=>
    string(1) "1"
    ["val:protected"]=>
    string(1) "A"
    ["val2:private"]=>
    NULL
    ["val2"]=>
    string(2) "AA"
  }
  [1]=>
  object(TestDerived)#%d (6) {
    ["p1:protected"]=>
    int(1)
    ["p2:protected"]=>
    int(2)
    ["id"]=>
    string(1) "2"
    ["val:protected"]=>
    string(1) "B"
    ["val2:private"]=>
    NULL
    ["val2"]=>
    string(2) "BB"
  }
  [2]=>
  object(TestDerived)#%d (6) {
    ["p1:protected"]=>
    int(1)
    ["p2:protected"]=>
    int(2)
    ["id"]=>
    string(1) "3"
    ["val:protected"]=>
    string(1) "C"
    ["val2:private"]=>
    NULL
    ["val2"]=>
    string(2) "CC"
  }
}
===DONE===
