--TEST--
ReflectionClass::isIterateable() basic
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>, Marc Veldman <marc@ibuildings.nl>
--FILE--
<?php

class IteratorClass implements Iterator {
	public function __construct() { }
	public function key() {}
	public function current() {}
	function next()	{}
	function valid() {}
	function rewind() {}
}
class DerivedClass extends IteratorClass {}
class NonIterator {}

function dump_iterateable($class) {
	$reflection = new ReflectionClass($class);
	var_dump($reflection->isIterateable());
}

$classes = array("ArrayObject", "IteratorClass", "DerivedClass", "NonIterator");
foreach ($classes as $class) {
	echo "Is $class iterateable? ";
	dump_iterateable($class);
}
?>
--EXPECT--
Is ArrayObject iterateable? bool(true)
Is IteratorClass iterateable? bool(true)
Is DerivedClass iterateable? bool(true)
Is NonIterator iterateable? bool(false)
