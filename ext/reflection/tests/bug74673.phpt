--TEST--
Bug #74673 (Segfault when cast Reflection object to string with undefined constant)
--FILE--
<?php

class A
{
	public function method($test = PHP_SELF + 1)
	{
	}
}

$class = new ReflectionClass('A');

echo $class;
?>
--EXPECTF--
Fatal error: Method ReflectionClass::__toString() must not throw an exception, caught Error: Undefined constant 'PHP_SELF' in %s on line %d
