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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcsub('1','2',-1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcmul('1','2',-1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcdiv('1','2',-1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcmod('1','2',-1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcpowmod('1', '2', '3', -9);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcpow('1', '2', -1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcsqrt('9', -1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bccomp('1', '2', -1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    bcscale(-1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
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
