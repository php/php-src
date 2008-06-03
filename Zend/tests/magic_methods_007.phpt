--TEST--
Testing __set() declaration in abstract class with wrong modifier
--FILE--
<?php

abstract class b {
	abstract protected function __set($a);
}

?>
--EXPECTF--
Fatal error: The magic method __set() must have public visibility and can not be static in %s on line %d
