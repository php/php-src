--TEST--
gmp_fact() rejects values larger than unsigned long
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    var_dump(gmp_fact(gmp_pow(2, 100)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gmp_fact(gmp_init("18446744073709551616")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
gmp_fact(): Argument #1 ($num) must be between 0 and %d
gmp_fact(): Argument #1 ($num) must be between 0 and %d
Done
