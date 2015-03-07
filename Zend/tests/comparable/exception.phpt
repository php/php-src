--TEST--
Comparable: throwing an exception within compareTo()
--FILE--
<?php

class C implements Comparable {
	public function compareTo($other) {
		throw new Exception;
	}
}

var_dump(new C <=> new C);

?>
--EXPECTF--
Fatal error: Uncaught exception 'Exception' in %s:%d
Stack trace:
#0 %s(%d): C->compareTo(Object(C))
#1 {main}
  thrown in %s on line %d
