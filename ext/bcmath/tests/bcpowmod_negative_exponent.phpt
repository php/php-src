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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: bcpowmod(): Argument #2 ($exponent) must be greater than or equal to 0
