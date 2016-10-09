--TEST--
Bug #70644: trivial hash complexity DoS attack (array_column)
--FILE--
<?php

$a = [];
$s = 1 << 16;
for ($i = 0; count($a) < $s; $i += $s) {
	$a[] = ["k" => $i, "v" => 1];
}

var_dump(array_column($a, "v", "k"));

?>
--EXPECTF--

Fatal error: Uncaught HashCollisionError: Too many collisions in array in %s:%d
Stack trace:
#0 %s(%d): array_column(Array, 'v', 'k')
#1 {main}
  thrown in %s on line %d
