--TEST--
Trying inherit abstract function twice
--FILE--
<?php

interface d {
	static function B();	
}

interface c {
	function b();	
}

class_alias('c', 'w');

interface a extends d, w { }

?>
--EXPECTF--
Fatal error: Cannot make non static method c::B() static in class d in %s on line %d
