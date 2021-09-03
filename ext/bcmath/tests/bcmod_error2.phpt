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
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Modulo by zero
