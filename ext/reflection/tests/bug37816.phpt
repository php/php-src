--TEST--
Bug #37816 (ReflectionProperty does not throw exception when accessing protected attribute)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class TestClass
{
	protected $p = 2;
}

$o = new TestClass;

$r = new ReflectionProperty($o, 'p');

try
{
	$x = $r->getValue($o);
}
catch (Exception $e)
{
	echo 'Caught: ' . $e->getMessage() . "\n";
}

?>
===DONE===
--EXPECTF--
Caught: Cannot access non-public member TestClass::p
===DONE===
