--TEST--
ZE2 __clone()
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class MyCloneable {
	static $id = 0;

	function MyCloneable() {
		$this->id = self::$id++;
	}

	function __clone() {
		$this->name = $that->name;
		$this->address = "New York";
		$this->id = self::$id++;
	}
}

$original = new MyCloneable();

$original->name = "Hello";
$original->address = "Tel-Aviv";

echo $original->id . "\n";

$clone = $original->__clone();

echo $clone->id . "\n";
echo $clone->name . "\n";
echo $clone->address . "\n";

?>
--EXPECT--
0
1
Hello
New York
