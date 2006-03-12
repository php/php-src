--TEST--
ReflectionMethod::invoke() with base class method
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class Foo
{
	function Test()
	{
		echo __METHOD__ . "\n";
	}
}

class Bar extends Foo
{
	function Test()
	{
		echo __METHOD__ . "\n";
	}
}

$o = new Bar;
$r = new ReflectionMethod('Foo','Test');

$r->invoke($o);

?>
===DONE===
--EXPECT--
Foo::Test
===DONE===
