--TEST--
ZE2 interface and __construct
--FILE--
<?php

class MyObject {}

interface MyInterface
{
	public function __construct(MyObject $o);
}

class MyTestClass implements MyInterface
{
	public function __construct(MyObject $o)
	{
	}
}

$obj = new MyTestClass;

?>
===DONE===
--EXPECTF--

Fatal error: Uncaught TypeError: Argument 1 passed to MyTestClass::__construct() must be an instance of MyObject, none given, called in %sinterfaces_003.php:%d
Stack trace:
#0 %s(%d): MyTestClass->__construct()
#1 {main}
  thrown in %sinterfaces_003.php on line %d
