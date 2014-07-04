--TEST--
GMP casting using casting operators
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$n = gmp_init(42);
echo $n, "\n";
var_dump((string) $n);
var_dump((int) $n);
var_dump((float) $n);
var_dump((bool) $n);

?>
--EXPECTF--	
42
string(2) "42"
int(42)
float(42)

Catchable fatal error: Object of class GMP could not be converted to boolean in %s on line %d
