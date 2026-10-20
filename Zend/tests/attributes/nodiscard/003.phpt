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
Warning: The return value of method Clazz::test() should either be used or intentionally ignored by casting it as (void) in %s on line %d
