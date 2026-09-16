--TEST--
#[\NoDiscard]: Not allowed on never function.
--FILE--
<?php

#[\NoDiscard]
function test(): never {
	throw new \Exception('Error!');
}

test();

?>
--EXPECTF--
Fatal error: A never returning function does not return a value, but #[\NoDiscard] requires a return value in %s on line %d
