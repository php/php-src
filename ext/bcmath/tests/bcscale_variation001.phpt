--TEST--
bcscale() fails with negative argument
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcdiv("20.56", "4");
try {
    bcscale(-4);
} catch (\ValueError $e) {
    echo \PHP_EOL . $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
5
bcscale(): Argument #1 ($scale) must be between 0 and 2147483647
