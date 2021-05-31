--TEST--
bcscale related problem on 64bits platforms
--EXTENSIONS--
bcmath
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
try {
    $var48 = bcscale(634314234334311);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
$var67 = bcsqrt(0);
$var414 = bcadd(0,-1,10);
?>
--EXPECT--
bcscale(): Argument #1 ($scale) must be between 0 and 2147483647
