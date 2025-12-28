--TEST--
GH-19653 (Closure named argument unpacking between temporary closures can cause a crash)
--CREDITS--
ivan-u7n
--FILE--
<?php

function func1(string $a1 = 'a1', string $a2 = 'a2', string $a3 = 'a3') {
	echo __FUNCTION__ . "() a1=$a1 a2=$a2 a3=$a3\n";
}

function usage1(?Closure $func1 = null) {
	echo __FUNCTION__ . "() ";
	($func1 ?? func1(...))(a3: 'm3+');
}
usage1();

$func1 = function (string ...$args) {
	echo "[function] ";
	func1(...$args, a2: 'm2+');
};
usage1(func1: $func1);

?>
--EXPECT--
usage1() func1() a1=a1 a2=a2 a3=m3+
usage1() [function] func1() a1=a1 a2=m2+ a3=m3+
