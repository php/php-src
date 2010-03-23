--TEST--
Testing __toString() declaration with wrong modifier
--FILE--
<?php

class a {
	static protected function __toString($a, $b) {
	}
}

?>
--EXPECTF--
Warning: The magic method __toString() must have public visibility and cannot be static in %s on line %d

Fatal error: Method a::__tostring() cannot take arguments in %s on line %d
