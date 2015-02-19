--TEST--
Comparable: compareTo() method incompatible
--FILE--
<?php

class C implements Comparable {
	public function compareTo(C $other) {}
}

?>
--EXPECTF--
Fatal error: Declaration of C::compareTo() must be compatible with Comparable::compareTo($other) in %s on line %d
