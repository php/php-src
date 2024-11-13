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
?>
--EXPECT--
bcround(): Argument #1 ($num) is not well-formed
bcround(): Argument #1 ($num) is not well-formed
