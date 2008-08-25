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
Fatal error: Can't inherit abstract function c::B() (previously declared abstract in d) in %s on line %d
