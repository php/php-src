--TEST--
GH-16870 (gmp_pow(64, 11) throws overflow exception)
--EXTENSIONS--
gmp
--FILE--
<?php
var_dump((string) gmp_pow(64, 11));
try {
    gmp_pow("18446744073709551616", 0x7fffffff);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(20) "73786976294838206464"
base and exponent overflow
