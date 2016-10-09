--TEST--
Bug #70644: trivial hash complexity DoS attack (array_merge)
--FILE--
<?php

$s = 999;
$len = ceil(log(2 * $s, 8));
$perm = ["E\x7a", "F\x59", "G\x38", "H\x17", "I\xf6", "J\xd5", "K\xb4", "L\x93"];
$a = $b = [];
$j = 0;
foreach (["a", "b"] as $var) {
	for ($i = 0; $i < $s; $i++, $j++) {
		$key = "";
		for ($k = 0; $k < $len; $k++) {
			$key .= $perm[($j >> (3 * $k)) % 8];
		}
		$$var[$key] = "1";
	}
}
var_dump(array_merge($a, $b));

?>
--EXPECTF--
Fatal error: Uncaught HashCollisionError: Too many collisions in array in %s:%d
Stack trace:
#0 %s(%d): array_merge(Array, Array)
#1 {main}
  thrown in %s on line %d
