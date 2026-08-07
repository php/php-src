--TEST--
bcscale() fails with negative argument
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcdiv("20.56", "4"), PHP_EOL;
try {
    bcscale(-4);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
5
ValueError: bcscale(): Argument #1 ($scale) must be between 0 and 2147483647
