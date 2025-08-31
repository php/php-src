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
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcsub('1','2',-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcmul('1','2',-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcdiv('1','2',-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcmod('1','2',-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcpowmod('1', '2', '3', -9);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcpow('1', '2', -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcsqrt('9', -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bccomp('1', '2', -1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcscale(-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcadd(): Argument #3 ($scale) must be between 0 and 2147483647
bcsub(): Argument #3 ($scale) must be between 0 and 2147483647
bcmul(): Argument #3 ($scale) must be between 0 and 2147483647
bcdiv(): Argument #3 ($scale) must be between 0 and 2147483647
bcmod(): Argument #3 ($scale) must be between 0 and 2147483647
bcpowmod(): Argument #4 ($scale) must be between 0 and 2147483647
bcpow(): Argument #3 ($scale) must be between 0 and 2147483647
bcsqrt(): Argument #2 ($scale) must be between 0 and 2147483647
bccomp(): Argument #3 ($scale) must be between 0 and 2147483647
bcscale(): Argument #1 ($scale) must be between 0 and 2147483647
