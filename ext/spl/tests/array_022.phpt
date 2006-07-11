--TEST--
SPL: ArrayObject/Iterator and reference to self
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
==ArrayObject===
<?php

class MyArrayObject extends ArrayObject
{
	public function __construct()
	{
		parent::__construct($this);
		$this['bar'] = 'baz';
	}
}

$a = new MyArrayObject;

$b = clone $a;
$b['baz'] = 'Foo';

var_dump($a);
var_dump($b);

?>
==ArrayIterator===
<?php

class MyArrayIterator extends ArrayIterator
{
	public function __construct()
	{
		parent::__construct($this);
		$this['bar'] = 'baz';
	}
}

$a = new MyArrayIterator;

$b = clone $a;
$b['baz'] = 'Foo';

var_dump($a);
var_dump($b);

?>
===DONE===
--EXPECTF--	
==ArrayObject===
object(MyArrayObject)#%d (1) {
  ["bar"]=>
  string(3) "baz"
}
object(MyArrayObject)#%d (2) {
  ["bar"]=>
  string(3) "baz"
  ["baz"]=>
  string(3) "Foo"
}
==ArrayIterator===
object(MyArrayIterator)#%d (1) {
  ["bar"]=>
  string(3) "baz"
}
object(MyArrayIterator)#%d (2) {
  ["bar"]=>
  string(3) "baz"
  ["baz"]=>
  string(3) "Foo"
}
===DONE===
