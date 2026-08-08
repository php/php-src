--TEST--
all errors on negative scale
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
try {
    bcadd('1','2',-1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcsub('1','2',-1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcmul('1','2',-1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcdiv('1','2',-1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcmod('1','2',-1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcpowmod('1', '2', '3', -9);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcpow('1', '2', -1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcsqrt('9', -1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bccomp('1', '2', -1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    bcscale(-1);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: bcadd(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcsub(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcmul(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcdiv(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcmod(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcpowmod(): Argument #4 ($scale) must be between 0 and 2147483647
ValueError: bcpow(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcsqrt(): Argument #2 ($scale) must be between 0 and 2147483647
ValueError: bccomp(): Argument #3 ($scale) must be between 0 and 2147483647
ValueError: bcscale(): Argument #1 ($scale) must be between 0 and 2147483647
