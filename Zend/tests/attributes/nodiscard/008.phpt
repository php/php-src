--TEST--
#[\NoDiscard]: Combining with #[\Deprecated].
--FILE--
<?php

#[\NoDiscard]
#[\Deprecated]
function test(): int {
	return 0;
}

test();

?>
--EXPECTF--
Deprecated: Function test() is deprecated in %s on line %d

Warning: The return value of function test() should either be used or intentionally ignored by casting it as (void) in %s on line %d
