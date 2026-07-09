--TEST--
GMP operator right operand rejects values outside the unsigned long range
--EXTENSIONS--
gmp
--FILE--
<?php
$too_large = gmp_init("18446744073709551616");

try {
    var_dump(gmp_init(2) ** $too_large);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump(gmp_init(2) << $too_large);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
Exponent must be between 0 and %d
Shift must be between 0 and %d
Done
