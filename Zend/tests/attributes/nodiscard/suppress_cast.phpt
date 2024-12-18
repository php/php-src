--TEST--
#[\NoDiscard]: Casting to (bool) suppresses.
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

(bool)test();
(bool)test2();
(bool)test3(1, 2, named: 3);
(bool)call_user_func("test");
$fcc = test(...);
(bool)$fcc();

$cls = new Clazz();
(bool)$cls->test();
(bool)$cls->test2();
(bool)call_user_func([$cls, "test"]);
(bool)Clazz::test3();

(bool)$closure();

(bool)$closure2();

?>
DONE
--EXPECT--
DONE
