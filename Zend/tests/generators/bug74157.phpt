--TEST--
Bug #74157 (Segfault with nested generators)
--FILE--
<?php

function a() {
	$a = $b = $c = 2;
	foreach(range(1, 5) as $v) {
		yield $v;
	}
	return;
}

foreach (a(range(1, 3)) as $a) {
	var_dump($a);
}
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
