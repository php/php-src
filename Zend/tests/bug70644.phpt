--TEST--
Bug #70644: trivial hash complexity DoS attack (plain array)
--FILE--
<?php

$a = [];
$s = 1 << 16;
for ($i = 0; count($a) < $s; $i += $s) {
	$a[$i] = 0;
}

?>
--EXPECTF--

Fatal error: Uncaught HashCollisionError: Too many collisions in array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
