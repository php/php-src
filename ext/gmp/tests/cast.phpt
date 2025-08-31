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
var_dump((bool) $n);

echo "\n";

$zero = gmp_init(0);
echo $zero, "\n";
var_dump((string) $zero);
var_dump((int) $zero);
var_dump((float) $zero);
var_dump((bool) $zero);

?>
--EXPECT--
42
string(2) "42"
int(42)
float(42)
bool(true)

0
string(1) "0"
int(0)
float(0)
bool(false)
