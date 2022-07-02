--TEST--
gmp_random_bits() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    var_dump(gmp_random_bits(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_random_bits(-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// If these error the test fails.
gmp_random_bits(1);
gmp_random_bits(1024);

// 0.5 seconds to make sure the numbers stay in range
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

    if (microtime(true) - $start > 0.5) {
        break;
    }
}

echo "Done\n";
?>
--EXPECT--
gmp_random_bits(): Argument #1 ($bits) must be greater than or equal to 1
gmp_random_bits(): Argument #1 ($bits) must be greater than or equal to 1
Done
