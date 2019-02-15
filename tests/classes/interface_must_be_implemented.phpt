--TEST--
ZE2 An interface must be implemented
--FILE--
<?php

interface if_a {
	function f_a();
}

class derived_a implements if_a {
}

?>
--EXPECTF--
Fatal error: Class derived_a contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (if_a::f_a) in %s on line %d
