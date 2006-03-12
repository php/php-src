--TEST--
ReflectionClass::newInstance[Args]
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

function test($class)
{
	echo "====>$class\n";
	try
	{
		$ref = new ReflectionClass($class);
	}
	catch (ReflectionException $e)
	{
		var_dump($e->getMessage());
		return; // only here
	}

	echo "====>newInstance()\n";
	try
	{
		var_dump($ref->newInstance());
	}
	catch (ReflectionException $e)
	{
		var_dump($e->getMessage());
	}
	
	echo "====>newInstance(25)\n";
	try
	{
		var_dump($ref->newInstance(25));
	}
	catch (ReflectionException $e)
	{
		var_dump($e->getMessage());
	}

	echo "====>newInstance(25, 42)\n";
	try
	{
		var_dump($ref->newInstance(25, 42));
	}
	catch (ReflectionException $e)
	{
		var_dump($e->getMessage());
	}
	
	echo "\n";
}

function __autoload($class)
{
	echo __FUNCTION__ . "($class)\n";
}

test('Class_does_not_exist');

Class NoCtor
{
}

test('NoCtor');

Class WithCtor
{
	function __construct()
	{
		echo __METHOD__ . "()\n";
		var_dump(func_get_args());
	}
}

test('WithCtor');

Class WithCtorWithArgs
{
	function __construct($arg)
	{
		echo __METHOD__ . "($arg)\n";
		var_dump(func_get_args());
	}
}

test('WithCtorWithArgs');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--

====>Class_does_not_exist
__autoload(Class_does_not_exist)
string(41) "Class Class_does_not_exist does not exist"
====>NoCtor
====>newInstance()
object(NoCtor)#%d (0) {
}
====>newInstance(25)
string(86) "Class NoCtor does not have a constructor, so you cannot pass any constructor arguments"
====>newInstance(25, 42)
string(86) "Class NoCtor does not have a constructor, so you cannot pass any constructor arguments"

====>WithCtor
====>newInstance()
WithCtor::__construct()
array(0) {
}
object(WithCtor)#%d (0) {
}
====>newInstance(25)
WithCtor::__construct()
array(1) {
  [0]=>
  int(25)
}
object(WithCtor)#%d (0) {
}
====>newInstance(25, 42)
WithCtor::__construct()
array(2) {
  [0]=>
  int(25)
  [1]=>
  int(42)
}
object(WithCtor)#%d (0) {
}

====>WithCtorWithArgs
====>newInstance()

Warning: Missing argument 1 for WithCtorWithArgs::__construct() in %s007.php on line %d

Notice: Undefined variable: arg in %s007.php on line %d
WithCtorWithArgs::__construct()
array(0) {
}
object(WithCtorWithArgs)#%d (0) {
}
====>newInstance(25)
WithCtorWithArgs::__construct(25)
array(1) {
  [0]=>
  int(25)
}
object(WithCtorWithArgs)#%d (0) {
}
====>newInstance(25, 42)
WithCtorWithArgs::__construct(25)
array(2) {
  [0]=>
  int(25)
  [1]=>
  int(42)
}
object(WithCtorWithArgs)#%d (0) {
}

===DONE===
