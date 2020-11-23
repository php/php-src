--TEST--
Feature Request #50283 (allow base in gmp_strval to use full range: 2 to 62, and -2 to -36)
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php
$a = gmp_init("0x41682179fbf5");
printf("Decimal: %s, -36-based: %s\n", gmp_strval($a), gmp_strval($a,-36));
printf("Decimal: %s, 36-based: %s\n", gmp_strval($a), gmp_strval($a,36));
try {
    printf("Decimal: %s, -1-based: %s\n", gmp_strval($a), gmp_strval($a,-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    printf("Decimal: %s, 1-based: %s\n", gmp_strval($a), gmp_strval($a,1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    printf("Decimal: %s, -37-based: %s\n", gmp_strval($a), gmp_strval($a,-37));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
printf("Decimal: %s, 37-based: %s\n", gmp_strval($a), gmp_strval($a,37));
printf("Decimal: %s, 62-based: %s\n", gmp_strval($a), gmp_strval($a,62));
try {
    printf("Decimal: %s, 63-based: %s\n\n", gmp_strval($a), gmp_strval($a,63));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
printf("Base 32 and 62-based: %s\n", gmp_strval(gmp_init("gh82179fbf5", 32), 62));
?>
--EXPECT--
Decimal: 71915494046709, -36-based: PHPISCOOL
Decimal: 71915494046709, 36-based: phpiscool
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
Decimal: 71915494046709, 37-based: KHKATELJF
Decimal: 71915494046709, 62-based: KQ6yq741
gmp_strval(): Argument #2 ($base) must be between 2 and 62, or -2 and -36
Base 32 and 62-based: 1NHkAcdIiD
