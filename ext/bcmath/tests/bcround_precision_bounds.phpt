--TEST--
bcround() and BcMath\Number::round() reject out-of-range $precision
--EXTENSIONS--
bcmath
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
try {
    bcround('1', PHP_INT_MAX);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcround('12345', -PHP_INT_MAX, RoundingMode::AwayFromZero);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    bcround('12345', PHP_INT_MIN, RoundingMode::AwayFromZero);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    (new BcMath\Number('1'))->round(PHP_INT_MAX);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    (new BcMath\Number('1'))->round(2147483648); // INT_MAX + 1
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
bcround(): Argument #2 ($precision) must be between %i and %i
bcround(): Argument #2 ($precision) must be between %i and %i
bcround(): Argument #2 ($precision) must be between %i and %i
BcMath\Number::round(): Argument #1 ($precision) must be between %i and %i
BcMath\Number::round(): Argument #1 ($precision) must be between %i and %i
