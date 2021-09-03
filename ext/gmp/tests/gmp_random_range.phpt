--TEST--
gmp_random_range() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

$minusTen = gmp_init(-10);
$plusTen = gmp_init(10);
$zero = gmp_init(0);

try {
    var_dump(gmp_random_range(10, -10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gmp_random_range($plusTen, $minusTen));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_random_range($plusTen, $zero));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

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
--EXPECT--
gmp_random_range(): Argument #1 ($min) must be less than argument #2 ($maximum)
gmp_random_range(): Argument #1 ($min) must be less than argument #2 ($maximum)
gmp_random_range(): Argument #1 ($min) must be less than argument #2 ($maximum)
Done
