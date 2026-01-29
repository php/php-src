--TEST--
pow() with GMP exponent too large should throw ValueError
--EXTENSIONS--
gmp
--FILE--
<?php
$a = gmp_init(10000000000);

try {
    pow($a, $a);
    echo "NO_EXCEPTION\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTREGEX--
Exponent must be less than [0-9]+