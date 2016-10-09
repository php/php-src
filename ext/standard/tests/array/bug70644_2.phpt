--TEST--
Bug #70644: trivial hash complexity DoS attack (array_fill_keys)
--FILE--
<?php

$a = [];
$s = 1 << 16;
for ($i = 0; count($a) < $s; $i += $s) {
	$a[] = $i;
}

var_dump(array_fill_keys($a, 0));

?>
--EXPECTF--

Fatal error: Uncaught HashCollisionError: Too many collisions in array in %s:%d
Stack trace:
#0 %s(%d): array_fill_keys(Array, 0)
#1 {main}
  thrown in %s on line %d
