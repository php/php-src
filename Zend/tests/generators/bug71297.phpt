--TEST--
Bug #71297 (Memory leak with consecutive yield from)
--FILE--
<?php

function foo() {
	yield array_fill(0, 10000, 4);
}

function genLeak() {
	$i = 0;
	while (1) {
		yield from foo();
		print $i++;
	}
}

$x = 0;
foreach (genLeak() as $i) {
	if ($x++ == 3) break;
}

?>
--EXPECT--
012
