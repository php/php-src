--TEST--
Comparable: compareTo() method not public
--FILE--
<?php

class C implements Comparable {
	protected function compareTo($other) {}
}

?>
--EXPECTF--
Fatal error: Access level to C::compareTo() must be public (as in class Comparable) in %s on line %d
