--TEST--
PDO-SQLite: PDO_FETCH_CLASS
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("pdo_sqlite")) print "skip"; ?>
--FILE--
<?php

$db =new pdo('sqlite::memory:');

$db->exec('CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10))');
$db->exec('INSERT INTO test VALUES(1, "A")'); 
$db->exec('INSERT INTO test VALUES(2, "B")'); 
$db->exec('INSERT INTO test VALUES(3, "C")'); 

class TestBase
{
	public $id;
	public $val;
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
  object(stdClass)#%d (2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  object(stdClass)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  [2]=>
  object(stdClass)#%d (2) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
}
array(3) {
  [0]=>
  object(TestBase)#%d (2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  object(TestBase)#%d (2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  [2]=>
  object(TestBase)#%d (2) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
}
array(3) {
  [0]=>
  object(TestDerived)#%d (4) {
    ["p1:protected"]=>
    int(1)
    ["p2:protected"]=>
    int(2)
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  object(TestDerived)#%d (4) {
    ["p1:protected"]=>
    int(1)
    ["p2:protected"]=>
    int(2)
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  [2]=>
  object(TestDerived)#%d (4) {
    ["p1:protected"]=>
    int(1)
    ["p2:protected"]=>
    int(2)
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
}
===DONE===
