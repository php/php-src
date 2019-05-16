--TEST--
Check if recursion with yield from works
--FILE--
<?php

function from($a = 0) {
	yield 1 + $a;
	if ($a <= 3) {
		yield from from($a + 3);
		yield from from($a + 6);
	}
	yield 2 + $a;
}

function gen() {
	yield from from();
}

foreach(gen() as $v) {
	var_dump($v);
}
?>
--EXPECT--
int(1)
int(4)
int(7)
int(8)
int(10)
int(11)
int(5)
int(7)
int(8)
int(2)
