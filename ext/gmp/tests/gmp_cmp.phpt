--TEST--
gmp_cmp() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_cmp(123123,-123123));
var_dump(gmp_cmp("12345678900987654321","12345678900987654321"));
var_dump(gmp_cmp("12345678900987654321","123456789009876543211"));
var_dump(gmp_cmp(0,0));
var_dump(gmp_cmp(1231222,0));
var_dump(gmp_cmp(0,345355));

$n = gmp_init("827278512385463739");
var_dump(gmp_cmp(0,$n) < 0);
$n1 = gmp_init("827278512385463739");
var_dump(gmp_cmp($n1,$n));

try {
    var_dump(gmp_cmp(array(),array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
int(2)
int(0)
int(-1)
int(0)
int(1)
int(-1)
bool(true)
int(0)
gmp_cmp(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
