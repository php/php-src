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
Warning: (B)The return value of function test() is expected to be consumed, 1234 in %s on line %d
