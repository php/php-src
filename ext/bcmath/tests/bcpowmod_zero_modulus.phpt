--TEST--
bc_raisemod's mod can't be zero
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    var_dump(bcpowmod('1', '1', '0'));
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
DivisionByZeroError: Modulo by zero
