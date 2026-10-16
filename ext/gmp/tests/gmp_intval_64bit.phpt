--TEST--
gmp_intval() on 64bit int
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) print "skip, this test is for 64bit int only";
?>
--EXTENSIONS--
gmp
--FILE--
<?php
/* Where zend_long is wider than long, GMP's mpz_set_si()/mpz_get_si() cannot
 * carry the whole int range and gmp.c converts through mpz_import() and
 * mpz_export() instead. Every int has to survive the round trip unchanged. */
$values = [
    /* the largest magnitudes a signed 32bit long still holds */
    2147483647, -2147483648,
    /* the first ones it does not */
    2147483648, -2147483649,
    3000000000, -3000000000,
    4294967295, -4294967296,
    PHP_INT_MAX, PHP_INT_MIN,
];

foreach ($values as $value) {
    $num = gmp_init($value);
    printf("%d: %s %d\n", $value, gmp_strval($num), gmp_intval($num));
}

echo "\n";

/* Numbers too large for an int. Which value comes back is not specified, but
 * it has to be the low PHP_INT_SIZE * 8 bits and not an uninitialised stack
 * slot: mpz_export() writes nothing at all when those bits are all zero. */
$fixtures = [
    '2 ** 64'     => gmp_pow(2, 64),
    '2 ** 65'     => gmp_pow(2, 65),
    '2 ** 128'    => gmp_pow(2, 128),
    '2 ** 64 + 1' => gmp_add(gmp_pow(2, 64), 1),
];

foreach ($fixtures as $label => $num) {
    echo "$label: ";
    var_dump(gmp_intval($num));
}
?>
--EXPECT--
2147483647: 2147483647 2147483647
-2147483648: -2147483648 -2147483648
2147483648: 2147483648 2147483648
-2147483649: -2147483649 -2147483649
3000000000: 3000000000 3000000000
-3000000000: -3000000000 -3000000000
4294967295: 4294967295 4294967295
-4294967296: -4294967296 -4294967296
9223372036854775807: 9223372036854775807 9223372036854775807
-9223372036854775808: -9223372036854775808 -9223372036854775808

2 ** 64: int(0)
2 ** 65: int(0)
2 ** 128: int(0)
2 ** 64 + 1: int(1)
