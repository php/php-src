--TEST--
Bug GH-9308: GMP throws the wrong error when a GMP object is passed to gmp_init()
--EXTENSIONS--
gmp
--FILE--
<?php
declare(strict_types=1);

/* We need strict_types as GMP has a __toString() handler */

try {
    $gmp = gmp_init(gmp_init(123));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
gmp_init(): Argument #1 ($num) must be of type string|int, GMP given
