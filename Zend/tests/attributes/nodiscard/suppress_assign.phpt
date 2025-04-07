--TEST--
#[\NoDiscard]: Assigning to variable suppresses.
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

$_ = test();
$_ = test2();
$_ = test3(1, 2, named: 3);
$_ = call_user_func("test");
$fcc = test(...);
$_ = $fcc();

$cls = new Clazz();
$_ = $cls->test();
$_ = $cls->test2();
$_ = call_user_func([$cls, "test"]);
$_ = Clazz::test3();

$_ = $closure();

$_ = $closure2();

?>
DONE
--EXPECT--
DONE
