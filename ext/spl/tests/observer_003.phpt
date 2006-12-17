--TEST--
SPL: SplObjectStorage serialization
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
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
--EXPECT--
int(4)
int(1)
string(1) "2"
string(3) "foo"
bool(true)
===UNSERIALIZE===
int(4)
int(1)
string(1) "2"
string(3) "foo"
bool(true)
===DONE===
--UEXPECT--
int(4)
int(1)
unicode(1) "2"
unicode(3) "foo"
bool(true)
===UNSERIALIZE===
int(4)
int(1)
unicode(1) "2"
unicode(3) "foo"
bool(true)
===DONE===
