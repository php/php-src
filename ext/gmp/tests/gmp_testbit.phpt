--TEST--
gmp_testbit() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$n = gmp_init(0);

try {
    var_dump(gmp_testbit($n, -10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_testbit($n, 0));
var_dump(gmp_testbit($n, 1));
var_dump(gmp_testbit($n, 100));

$n = gmp_init(-1);
var_dump(gmp_testbit($n, 1));
try {
    var_dump(gmp_testbit($n, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$n = gmp_init("1000000");
var_dump(gmp_testbit($n, 1));
gmp_setbit($n, 1);
var_dump(gmp_testbit($n, 1));
var_dump(gmp_strval($n));

gmp_setbit($n, 5);
var_dump(gmp_testbit($n, 5));
var_dump(gmp_strval($n));

$n = gmp_init("238462734628347239571823641234");
var_dump(gmp_testbit($n, 5));
gmp_setbit($n, 5);
var_dump(gmp_testbit($n, 5));
var_dump(gmp_strval($n));

gmp_clrbit($n, 5);
var_dump(gmp_testbit($n, 5));
var_dump(gmp_strval($n));

echo "Done\n";
?>
--EXPECT--
gmp_testbit(): Argument #2 ($index) must be greater than or equal to 0
bool(false)
bool(false)
bool(false)
bool(true)
gmp_testbit(): Argument #2 ($index) must be greater than or equal to 0
bool(false)
bool(true)
string(7) "1000002"
bool(true)
string(7) "1000034"
bool(false)
bool(true)
string(30) "238462734628347239571823641266"
bool(false)
string(30) "238462734628347239571823641234"
Done
