--TEST--
Bug #32773 (binary GMP functions returns unexpected value, when second parameter is int(0))
--EXTENSIONS--
gmp
--FILE--
<?php
echo '10 + 0 = ', gmp_strval(gmp_add(10, 0)), "\n";
echo '10 + "0" = ', gmp_strval(gmp_add(10, '0')), "\n";

try {
    var_dump(gmp_div(10, 0));
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_div_qr(10, 0));
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
10 + 0 = 10
10 + "0" = 10
Division by zero
Division by zero
