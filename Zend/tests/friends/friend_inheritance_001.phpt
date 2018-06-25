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

Fatal error: Uncaught Error: Cannot access protected property Derived::$property in /var/www/html/php-src/Zend/tests/friends/friend_inheritance_001.php:%d
Stack trace:
#0 /var/www/html/php-src/Zend/tests/friends/friend_inheritance_001.php(%d): Friendly->touch(Object(Derived))
#1 {main}
  thrown in /var/www/html/php-src/Zend/tests/friends/friend_inheritance_001.php on line %d