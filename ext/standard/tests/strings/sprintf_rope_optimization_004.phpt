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
	var_dump(sprintf("%d/%d/%d", $a, $b, $c));
} catch (\Throwable $e) {echo $e, PHP_EOL; } echo PHP_EOL;

echo "Done";
?>
--EXPECTF--
string(28) "42/-1337/4089650035136921599"

Done
