--TEST--
gmp_com() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_strval(gmp_com(0)));
var_dump(gmp_strval(gmp_com("0")));
try {
    var_dump(gmp_strval(gmp_com("test")));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_strval(gmp_com("2394876545678")));
var_dump(gmp_strval(gmp_com("-111")));
var_dump(gmp_strval(gmp_com(874653)));
var_dump(gmp_strval(gmp_com(-9876)));

$n = gmp_init("98765467");
var_dump(gmp_strval(gmp_com($n)));
$n = gmp_init("98765463337");
var_dump(gmp_strval(gmp_com($n)));

try {
    var_dump(gmp_strval(gmp_com(array())));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
string(2) "-1"
string(2) "-1"
gmp_com(): Argument #1 ($num) is not an integer string
string(14) "-2394876545679"
string(3) "110"
string(7) "-874654"
string(4) "9875"
string(9) "-98765468"
string(12) "-98765463338"
gmp_com(): Argument #1 ($num) must be of type GMP|string|int, array given
Done
