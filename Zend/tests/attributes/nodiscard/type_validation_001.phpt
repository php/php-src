--TEST--
#[\NoDiscard]: Type validation of $message parameter with int.
--FILE--
<?php

#[\NoDiscard(1234)]
function test(): int {
	return 0;
}

test();

?>
--EXPECTF--
Warning: The return value of function test() should either be used or intentionally ignored by casting it as (void), 1234 in %s on line %d
