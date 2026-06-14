--TEST--
#[\NoDiscard]: Casting to (void) suppresses.
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

(void)test();
(void)test2();
(void)test3(1, 2, named: 3);
(void)call_user_func("test");
$fcc = test(...);
(void)$fcc();

$cls = new Clazz();
(void)$cls->test();
(void)$cls->test2();
(void)call_user_func([$cls, "test"]);
(void)Clazz::test3();

(void)$closure();

(void)$closure2();

?>
DONE
--EXPECT--
DONE
