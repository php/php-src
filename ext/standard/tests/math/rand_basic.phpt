--TEST--
Test  rand() - basic function test rand()
--FILE--
<?php
$default_max = getrandmax();

echo "\nrand() tests with default min and max value (i.e 0 thru ", $default_max, ")\n";
for ($i = 0; $i < 100; $i++) {
	$res = rand();

// By default RAND_MAX is 32768 although no constant is defined for it for user space apps
	if (!is_int($res) || $res < 0 || $res > $default_max) {
		break;
	}
}

if ($i != 100) {
	echo "FAILED: res = ", $res, " min = 0 max = ", $default_max, "\n";
} else {
	echo "PASSED: range min = 0 max = ", $default_max, "\n";
}

echo "\nrand() tests with defined min and max value\n";

$min = array(10,
			 100,
			 10.5,
			 10.5e3,
			 0x10,
			 0400);

$max = array(100,
			 1000,
			 19.5,
			 10.5e5,
			 0x10000,
			 0700);

for ($x = 0; $x < count($min); $x++) {
	for ($i = 0; $i < 100; $i++) {
		$res = rand($min[$x], $max[$x]);

		if (!is_int($res) || $res < intval($min[$x]) || $res > intval($max[$x])) {
			echo "FAILED: res = ",  $res, " min = ", intval($min[$x]), " max = ", intval($max[$x]), "\n";
			break;
		}
	}

	if ($i == 100) {
		echo "PASSED: range min = ", intval($min[$x]), " max = ", intval($max[$x]), "\n";
	}
}

echo "\nNon-numeric cases\n";
$min = array(true,
			 false,
			 null,
			 "10",
			 "10.5");

// Eexepcted numerical equivalent of above non-numerics
$minval = array(1,
				0,
				0,
				10,
				10);
for ($x = 0; $x < count($min); $x++) {
	for ($i = 0; $i < 100; $i++) {
		$res = rand($min[$x], 100);

		if (!is_int($res) || $res < intval($minval[$x]) || $res > 100) {
			echo "FAILED: res = ",  $res, " min = ", intval($min[$x]), " max = ", intval($max[$x]), "\n";
			break;
		}
	}

	if ($i == 100) {
		echo "PASSED range min = ", intval($min[$x]), " max = 100\n";
	}
}
?>
--EXPECTF--
rand() tests with default min and max value (i.e 0 thru %i)
PASSED: range min = 0 max = %i

rand() tests with defined min and max value
PASSED: range min = 10 max = 100
PASSED: range min = 100 max = 1000
PASSED: range min = 10 max = 19
PASSED: range min = 10500 max = 1050000
PASSED: range min = 16 max = 65536
PASSED: range min = 256 max = 448

Non-numeric cases
PASSED range min = 1 max = 100
PASSED range min = 0 max = 100
PASSED range min = 0 max = 100
PASSED range min = 10 max = 100
PASSED range min = 10 max = 100
