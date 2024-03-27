--TEST--
bcround() function with error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    bcround('hoge');
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}

try {
    bcround('0.00.1');
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}

try {
    bcround('0.001', 0, 1000);
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECT--
bcround(): Argument #1 ($num) is not well-formed
bcround(): Argument #1 ($num) is not well-formed
bcround(): Argument #3 ($mode) must be a valid rounding mode (PHP_ROUND_*)
