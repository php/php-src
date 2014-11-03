--TEST--
gmp_random_bits() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_random_bits());
var_dump(gmp_random_bits(0));
var_dump(gmp_random_bits(-1));

// If these error the test fails.
gmp_random_bits(1);
gmp_random_bits(1024);

// 2 seconds to make sure the numbers stay in range
$start = microtime(true);
$limit = (2 ** 30) - 1;
while (1) {
	for ($i = 0; $i < 5000; $i++) {
		$result = gmp_random_bits(30);
		if ($result < 0 || $result > $limit) {
			print "RANGE VIOLATION\n";
			var_dump($result);
			break 2;
		}
	}

	if (microtime(true) - $start > 2) {
		break;
	}
}

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_random_bits() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: gmp_random_bits(): The number of bits must be positive in %s on line %d
bool(false)

Warning: gmp_random_bits(): The number of bits must be positive in %s on line %d
bool(false)
Done
