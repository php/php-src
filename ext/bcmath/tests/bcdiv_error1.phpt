--TEST--
bcdiv — Divide two arbitrary precision numbers
--CREDITS--
TestFest2009
Antoni Torrents
antoni@solucionsinternet.com
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    bcdiv('10.99', '0');
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}

try {
    bcdiv('10.99', '0.00');
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}

try {
    bcdiv('10.99', '-0.00');
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
DivisionByZeroError: Division by zero
DivisionByZeroError: Division by zero
DivisionByZeroError: Division by zero
