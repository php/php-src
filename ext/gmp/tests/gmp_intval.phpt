--TEST--
gmp_intval() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_intval("-1"));
var_dump(gmp_intval(-1));
var_dump(gmp_intval(-2349828));
var_dump(gmp_intval(2342344));
var_dump(gmp_intval(1.0001));

$g = gmp_init("12345678");
var_dump(gmp_intval($g));

try {
    var_dump(gmp_intval(""));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(gmp_intval(new stdclass));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(gmp_intval(array()));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(gmp_intval("1.0001"));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
int(-1)
int(-1)
int(-2349828)
int(2342344)
int(1)
int(12345678)
gmp_intval(): Argument #1 ($num) is not an integer string
gmp_intval(): Argument #1 ($num) must be of type GMP|string|int, stdClass given
gmp_intval(): Argument #1 ($num) must be of type GMP|string|int, array given
gmp_intval(): Argument #1 ($num) is not an integer string
Done
