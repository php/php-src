--TEST--
ZE2 interface and __construct
--FILE--
<?php

class MyObject {}

interface MyInterface
{
	public function __construct(Object $o);
}

class MyTestClass implements MyInterface
{
	public function __construct(Object $o)
	{
	}
}

$obj = new MyTestClass;

class MyTestFail
{
	public function __construct()
	{
	}
}

?>
===DONE===
--EXPECTF--

Fatal error: Argument 1 passed to MyTestClass::__construct() must be an object of class Object, called in %sinterfaces_003.php on line %d
