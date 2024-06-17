--TEST--
Test sprintf() function : Rope Optimization for '%d' with GMP objects
--EXTENSIONS--
gmp
--FILE--
<?php

$a = new GMP("42");
$b = new GMP("-1337");
$c = new GMP("999999999999999999999999999999999");

try {
	if (PHP_INT_SIZE == 8) {
		var_dump(sprintf("%d/%d/%d/%s", $a, $b, $c, $c + 1));
		var_dump("42/-1337/2147483647/1000000000000000000000000000000000");
	} else {
		var_dump("42/-1337/4089650035136921599/1000000000000000000000000000000000");
		var_dump(sprintf("%d/%d/%d/%s", $a, $b, $c, $c + 1));
	}
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

echo "Done";
?>
--EXPECTF--
string(63) "42/-1337/4089650035136921599/1000000000000000000000000000000000"
string(54) "42/-1337/2147483647/1000000000000000000000000000000000"

Done
