--TEST--
#[\NoDiscard]: Not allowed on void function.
--FILE--
<?php

#[\NoDiscard]
function test(): void {
	return;
}

test();

?>
--EXPECTF--
Fatal error: A void function does not return a value, but #[\NoDiscard] requires a return value in %s on line %d
