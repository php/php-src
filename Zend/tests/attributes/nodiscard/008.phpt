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

Warning: (F)The return value of function test() is expected to be consumed in %s on line %d
