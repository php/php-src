--TEST--
ZE2 object cloning, 6
--INI--
error_reporting=2047
--FILE--
<?php

class MyCloneable {
	static $id = 0;

	function __construct() {
		$this->id = self::$id++;
	}

	function __clone() {
		$this->address = "New York";
		$this->id = self::$id++;
	}
}

$original = new MyCloneable();

$original->name = "Hello";
$original->address = "Tel-Aviv";

echo $original->id . "\n";

$clone = clone $original;

echo $clone->id . "\n";
echo $clone->name . "\n";
echo $clone->address . "\n";

?>
--EXPECTF--
Notice: Accessing static property MyCloneable::$id as non static in %s on line %d

Notice: Accessing static property MyCloneable::$id as non static in %s on line %d
0

Notice: Accessing static property MyCloneable::$id as non static in %s on line %d

Notice: Accessing static property MyCloneable::$id as non static in %s on line %d
1
Hello
New York
