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
Fatal error: Uncaught ArgumentCountError: Too few arguments to function MyTestClass::__construct(), 0 passed in %sinterfaces_003.php on line 17 and exactly 1 expected in %sinterfaces_003.php:12
Stack trace:
#0 %s(%d): MyTestClass->__construct()
#1 {main}
  thrown in %sinterfaces_003.php on line %d
