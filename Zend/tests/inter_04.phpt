--TEST--
Trying declare interface with repeated name of inherited method
--FILE--
<?php 

interface a {
	function b();
}

interface b {
	function b();
}

interface c extends a, b {
}

?>
--EXPECTF--
Fatal error: Can't inherit abstract function b::b() (previously declared abstract in a) in %s on line %d
