--TEST--
ZE2 An interface cannot be instantiated
--FILE--
<?php

interface if_a {
	function f_a();
}

$t = new if_a();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate interface if_a in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
