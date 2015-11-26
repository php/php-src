--TEST--
Trying to use an interface as trait
--FILE--
<?php

interface abc { 
}

class A {
	use abc;
}

?>
--EXPECTF--
Fatal error: A cannot use abc - it is not a trait in %s on line %d
