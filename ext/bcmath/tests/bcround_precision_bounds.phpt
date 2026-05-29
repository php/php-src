--TEST--
bcround() and BcMath\Number::round() reject out-of-range $precision
--EXTENSIONS--
bcmath
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
?>
--EXPECT--
bcround(): Argument #2 ($precision) must be between -2147483647 and 2147483647
bcround(): Argument #2 ($precision) must be between -2147483647 and 2147483647
bcround(): Argument #2 ($precision) must be between -2147483647 and 2147483647
BcMath\Number::round(): Argument #1 ($precision) must be between -2147483647 and 2147483647
