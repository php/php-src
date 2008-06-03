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
Fatal error: The magic method __toString() must have public visibility and can not be static in %s on line %d
