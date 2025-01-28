--TEST--
#[\NoDiscard]: Taken from trait.
--FILE--
<?php

trait T {
	#[\NoDiscard]
	function test(): int {
		return 0;
	}
}

class Clazz {
	use T;
}

$cls = new Clazz();
$cls->test();

?>
--EXPECTF--
Warning: The return value of method Clazz::test() is expected to be consumed in %s on line %d
