--TEST--
gmp_random_range() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$minusTen = gmp_init(-10);
$plusTen = gmp_init(10);
$zero = gmp_init(0);

var_dump(gmp_random_range());
var_dump(gmp_random_range(10));
var_dump(gmp_random_range(10, -10));

var_dump(gmp_random_range($plusTen, $minusTen));
var_dump(gmp_random_range($plusTen, $zero));

// If these error the test fails.
gmp_random_range(0, 10);
gmp_random_range(1, 10);
gmp_random_range(-1, 10);
gmp_random_range(-10, 0);
gmp_random_range(-10, -1);

gmp_random_range(0, $plusTen);
gmp_random_range(1, $plusTen);
gmp_random_range(-1, $plusTen);

gmp_random_range($zero, $plusTen);
gmp_random_range($minusTen, $plusTen);

// 0.5 seconds to make sure the numbers stay in range
$start = microtime(true);
while (1) {
	for ($i = 0; $i < 5000; $i++) {
		$result = gmp_random_range(0, 1000);
		if ($result < 0 || $result > 1000) {
			print "RANGE VIOLATION 1\n";
			var_dump($result);
			break 2;
		}

		$result = gmp_random_range(-1000, 0);
		if ($result < -1000 || $result > 0) {
			print "RANGE VIOLATION 2\n";
			var_dump($result);
			break 2;
		}

		$result = gmp_random_range(-500, 500);
		if ($result < -500 || $result > 500) {
			print "RANGE VIOLATION 3\n";
			var_dump($result);
			break 2;
		}
	}

	if (microtime(true) - $start > 0.5) {
		break;
	}
}

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_random_range() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: gmp_random_range() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: gmp_random_range(): The minimum value must be less than the maximum value in %s on line %d
bool(false)

Warning: gmp_random_range(): The minimum value must be less than the maximum value in %s on line %d
bool(false)

Warning: gmp_random_range(): The minimum value must be less than the maximum value in %s on line %d
bool(false)
Done
