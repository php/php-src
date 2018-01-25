--TEST--
SPL: SplObjectStorage serialization
--FILE--
<?php

class TestClass
{
	public $test = 25;

	public function __construct($test = 42)
	{
		$this->test = $test;
	}
}

$storage = new SplObjectStorage();

foreach(array(1,"2","foo",true) as $value)
{
     $storage->attach(new TestClass($value));
}

var_dump(count($storage));

foreach($storage as $object)
{
	var_dump($object->test);
}

var_dump(serialize($storage));
echo "===UNSERIALIZE===\n";

$storage2 = unserialize(serialize($storage));

var_dump(count($storage2));

foreach($storage2 as $object)
{
	var_dump($object->test);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(4)
int(1)
string(1) "2"
string(3) "foo"
bool(true)
string(%d) "%s"
===UNSERIALIZE===
int(4)
int(1)
string(1) "2"
string(3) "foo"
bool(true)
===DONE===
