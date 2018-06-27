--TEST--
A friend of class Base is not automatically a friend of class Derived and vice-versa
--FILE--
<?php

class Base
{
	friend Friendly;

	protected $property = 'base';
}

class Derived extends Base
{
	protected $property = 'derived';
}

class Friendly
{
	public function touch(Base $base)
	{
		echo $base->property . PHP_EOL;
	}
}

$base = new Base();
$derived = new Derived();
$friend = new Friendly();

$friend->touch($base);
$friend->touch($derived);

?>
--EXPECTF--
base

Fatal error: Uncaught Error: Cannot access protected property Derived::$property in %s:%d
Stack trace:
#0 %s(%d): Friendly->touch(Object(Derived))
#1 {main}
  thrown in %s on line %d