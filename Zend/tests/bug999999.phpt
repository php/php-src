--TEST--
Bug #999999 ZPP int/float/number specifier doesn't accept numeric objets
--EXTENSIONS--
gmp
--FILE--
<?php
var_dump(abs(gmp_init(-3)));
var_dump(exp(gmp_init(2)));
var_dump(base_convert('10', gmp_init(2), 10));
?>
--EXPECT--
int(3)
float(7.38905609893065)
string(1) "2"
