--TEST--
GMP casting using casting operators
--EXTENSIONS--
gmp
--FILE--
<?php

$n = gmp_init(42);
echo $n, "\n";
var_dump((string) $n);
var_dump((int) $n);
var_dump((float) $n);
try {
    var_dump((bool) $n);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
42
string(2) "42"
int(42)
float(42)
Object of class GMP could not be converted to bool
