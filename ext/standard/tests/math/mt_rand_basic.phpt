--TEST--
Test mt_rand() - basic function test mt_rand()
--FILE--
<?php
$default_max = mt_getrandmax();

echo "\nmt_rand() tests with default min and max value (i.e 0 thru ", $default_max, ")\n";
for ($i = 0; $i < 100; $i++) {
    $res = mt_rand();

// By default RAND_MAX is 32768 although no constant is defined for it for user space apps
    if ($res < 0 || $res > $default_max) {
        break;
    }
}

if ($i != 100) {
    echo "FAILED: res = ",  $res, " min = 0 max = ", $default_max, "\n";
} else {
    echo "PASSED: range min = 0  max = ", $default_max, "\n";
}

echo "\nmt_rand() tests with defined min and max value\n";

$min = array(10,
             100,
             10.5e3,
             0x10,
             0400);

$max = array(100,
             1000,
             10.5e5,
             0x10000,
             0700);

for ($x = 0; $x < count($min); $x++) {
    for ($i = 0; $i < 100; $i++) {
        $res = mt_rand($min[$x], $max[$x]);

        if (!is_int($res) || $res < intval($min[$x]) || $res > intval($max[$x])) {
            echo "FAILED: res = ",  $res, " min = ", intval($min[$x]), " max = ", intval($max[$x]), "\n";
            break;
        }
    }

    if ($i == 100) {
        echo "PASSED: range min = ", intval($min[$x]), " max = ", intval($max[$x]), "\n";
    }
}

?>
--EXPECT--
mt_rand() tests with default min and max value (i.e 0 thru 2147483647)
PASSED: range min = 0  max = 2147483647

mt_rand() tests with defined min and max value
PASSED: range min = 10 max = 100
PASSED: range min = 100 max = 1000
PASSED: range min = 10500 max = 1050000
PASSED: range min = 16 max = 65536
PASSED: range min = 256 max = 448

