--TEST--
bcmod() - mod by 0
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
try {
    bcmod("10", "0");
} catch (DivisionByZeroError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), PHP_EOL;
}
try {
    bcmod("10", "0.000");
} catch (DivisionByZeroError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), PHP_EOL;
}
try {
    bcmod("10", "-0.0");
} catch (DivisionByZeroError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
DivisionByZeroError: Modulo by zero
