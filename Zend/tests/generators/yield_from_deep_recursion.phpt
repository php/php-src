--TEST--
Deep recursion with yield from
--FILE--
<?php
ini_set("memory_limit", "512M");

function from($i) {
	yield $i;
}

function gen($i = 0) {
	if ($i < 50000) {
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
int(50000)
int(50001)
