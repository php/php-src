--TEST--
SPL: RecursiveRegexIterator and exception in has/getChildren
--FILE--
<?php

class MyRecursiveRegexIterator extends RecursiveRegexIterator
{
	function show()
	{
		foreach(new RecursiveIteratorIterator($this) as $k => $v)
		{
			var_dump($k);
			var_dump($v);
		}
	}

	function accept()
	{
		return $this->hasChildren() || parent::accept();
	}
}

$ar = new RecursiveArrayIterator(array('Foo', array('Bar'), 'FooBar', array('Baz'), 'Biz'));
$it = new MyRecursiveRegexIterator($ar, '/Bar/');

$it->show();

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(0)
string(3) "Bar"
int(2)
string(6) "FooBar"
===DONE===
