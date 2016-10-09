--TEST--
Bug #70644: trivial hash complexity DoS attack (array_combine)
--FILE--
<?php

$a = [];
$s = 1 << 16;
for ($i = 0; count($a) < $s; $i += $s) {
	$a[] = $i;
}

var_dump(array_combine($a, array_fill(0, $s, 0)));

?>
--EXPECTF--

Fatal error: Uncaught HashCollisionError: Too many collisions in array in %s:%d
Stack trace:
#0 %s(%d): array_combine(Array, Array)
#1 {main}
  thrown in %s on line %d
