--TEST--
gmp_kronecker(): Kronecker symbol
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_kronecker(23, 12));
var_dump(gmp_kronecker(gmp_init(23), 12));
var_dump(gmp_kronecker(23, gmp_init(12)));
var_dump(gmp_kronecker(gmp_init(23), gmp_init(12)));
var_dump(gmp_kronecker("23", 12));
var_dump(gmp_kronecker(23, "12"));
var_dump(gmp_kronecker("23", "12"));
echo "\n";

var_dump(gmp_kronecker(23, -12));
var_dump(gmp_kronecker(-23, 12));
var_dump(gmp_kronecker(-23, -12));

?>
--EXPECT--
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)

int(-1)
int(1)
int(-1)
