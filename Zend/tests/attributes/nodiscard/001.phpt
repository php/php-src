--TEST--
#[\NoDiscard]: Basic test.
--FILE--
<?php

#[\NoDiscard]
function test(): int {
	return 0;
}

#[\NoDiscard("this is important")]
function test2(): int {
	return 0;
}

#[\NoDiscard]
function test3(...$args): int {
	return 0;
}

class Clazz {
	#[\NoDiscard]
	function test(): int {
		return 0;
	}

	#[\NoDiscard("this is important")]
	function test2(): int {
		return 0;
	}

	#[\NoDiscard]
	static function test3(): int {
		return 0;
	}
}

$closure = #[\NoDiscard] function(): int {
	return 0;
};

$closure2 = #[\NoDiscard] function(): int {
	return 0;
};

test();
test2();
test3(1, 2, named: 3);
call_user_func("test");
$fcc = test(...);
$fcc();

$cls = new Clazz();
$cls->test();
$cls->test2();
Clazz::test3();

call_user_func([$cls, "test"]);

$closure();

$closure2();

?>
--EXPECTF--
Warning: The return value of function test() is expected to be consumed in %s on line %d

Warning: The return value of function test2() is expected to be consumed, this is important in %s on line %d

Warning: The return value of function test3() is expected to be consumed in %s on line %d

Warning: The return value of function test() is expected to be consumed in %s on line %d

Warning: The return value of function test() is expected to be consumed in %s on line %d

Warning: The return value of method Clazz::test() is expected to be consumed in %s on line %d

Warning: The return value of method Clazz::test2() is expected to be consumed, this is important in %s on line %d

Warning: The return value of method Clazz::test3() is expected to be consumed in %s on line %d

Warning: The return value of method Clazz::test() is expected to be consumed in %s on line %d

Warning: The return value of function {closure:%s:%d}() is expected to be consumed in %s on line %d

Warning: The return value of function {closure:%s:%d}() is expected to be consumed in %s on line %d
