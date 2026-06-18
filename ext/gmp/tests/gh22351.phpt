--TEST--
GH-22351: gmp_pow with PHP_INT_MAX should not crash
--EXTENSIONS--
gmp
--FILE--
<?php

echo "Testing gmp_pow overflow safety\n";

try {
    $r = gmp_pow(2, PHP_INT_MAX);
    var_dump($r);
} catch (ValueError $e) {
    echo "ValueError: " . $e->getMessage() . PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
Testing gmp_pow overflow safety
ValueError: gmp_pow(): Argument #2 ($exponent) results in a value that exceeds the supported size
Done
