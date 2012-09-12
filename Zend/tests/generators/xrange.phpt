--TEST--
Simple generator xrange() test
--FILE--
<?php

function xrange($start, $end, $step = 1) {
	for ($i = $start; $i <= $end; $i += $step) {
		yield $i;
	}
}

foreach (xrange(10, 20, 2) as $i) {
	var_dump($i);
}

?>
--EXPECT--
int(10)
int(12)
int(14)
int(16)
int(18)
int(20)
