--TEST--
bc_raisemod's expo can't be negative
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpowmod('1', '-1', '2'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bcpowmod(): Argument #2 ($exponent) must be greater than or equal to 0
