--TEST--
SPL: ArrayObject/Iterator and reference to self
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
  [u"bar"]=>
  unicode(3) "baz"
}
object(MyArrayObject)#%d (3) {
  [u"bar"]=>
  unicode(3) "baz"
  [u"baz"]=>
  unicode(3) "Foo"
  [u"storage":u"ArrayObject":private]=>
  array(1) {
    [u"bar"]=>
    unicode(3) "baz"
  }
}
==ArrayIterator===
object(MyArrayIterator)#%d (1) {
  [u"bar"]=>
  unicode(3) "baz"
}
object(MyArrayIterator)#%d (3) {
  [u"bar"]=>
  unicode(3) "baz"
  [u"baz"]=>
  unicode(3) "Foo"
  [u"storage":u"ArrayIterator":private]=>
  object(MyArrayIterator)#%d (1) {
    [u"bar"]=>
    unicode(3) "baz"
  }
}
===DONE===
