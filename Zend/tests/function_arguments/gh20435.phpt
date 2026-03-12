--TEST--
GH-20435 (SensitiveParameter doesn't work for named argument passing to variadic parameter)
--FILE--
<?php

function test($a, #[\SensitiveParameter] ...$x) {
	debug_print_backtrace();
}

test(b: 1, a: 2, c: 3);

?>
--EXPECTF--
#0 %s(%d): test(2, b: Object(SensitiveParameterValue), c: Object(SensitiveParameterValue))
