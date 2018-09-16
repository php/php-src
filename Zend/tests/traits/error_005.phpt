--TEST--
Trying to use a final class as trait
--FILE--
<?php

final class abc {
}

class A {
	use abc;
}

?>
--EXPECTF--
Fatal error: A cannot use abc - it is not a trait in %s on line %d
