--TEST--
Testing __call() declaration in interface with wrong modifier
--FILE--
<?php

interface a {
	static function __call($a, $b);
}

?>
--EXPECTF--
Fatal error: The magic method __call() must have public visibility and can not be static in %s on line %d
