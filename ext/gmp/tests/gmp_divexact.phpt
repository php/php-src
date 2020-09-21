--TEST--
gmp_divexact() tests
--SKIPIF--
<?php
if (!extension_loaded("gmp")) die ("skip");
if (!defined('GMP_VERSION') || version_compare("4.2.1", GMP_VERSION, ">=")) {
    die("skip your GMP is too old and will crash");
}
?>
--FILE--
<?php

$r = gmp_divexact("233", "23345555555555555555555555");
var_dump(gmp_strval($r));

try {
    $r = gmp_divexact("233", "0");
    var_dump(gmp_strval($r));
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$r = gmp_divexact("100", "10");
var_dump(gmp_strval($r));

$r = gmp_divexact("1024", "2");
var_dump(gmp_strval($r));

$n = gmp_init("10000000000000000000");
$r = gmp_divexact($n, "2");
var_dump(gmp_strval($r));

$r = gmp_divexact($n, "50");
var_dump(gmp_strval($r));

$n1 = gmp_init("-100000000000000000000000000");
$r = gmp_divexact($n1, $n);
var_dump(gmp_strval($r));

echo "Done\n";
?>
--EXPECT--
string(1) "0"
Division by zero
string(2) "10"
string(3) "512"
string(19) "5000000000000000000"
string(18) "200000000000000000"
string(9) "-10000000"
Done
