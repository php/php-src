--TEST--
Comparable: compareTo() returns an object
--FILE--
<?php

class C implements Comparable {
	public function compareTo($other) {
		return $this;
	}
}

var_dump(new C <=> false);
var_dump(false <=> new C);

?>
--EXPECTF--
Notice: Object of class C could not be converted to int in %s on line %d
int(1)

Notice: Object of class C could not be converted to int in %s on line %d
int(-1)
