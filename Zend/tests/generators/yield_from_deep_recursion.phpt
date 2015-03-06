--TEST--
Deep recursion with yield from
--FILE--
<?php
ini_set("memory_limit", "60G");

function from($i) {
	yield $i;
}

function gen($i = 0) {
	if ($i < 1000) {
		yield from gen(++$i);
	} else {
		yield $i;
		yield from from(++$i);
	}
}

foreach (gen() as $v) {
	var_dump($v);
}
?>
--EXPECT--
int(1000)
int(1001)
