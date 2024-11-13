--TEST--
bcfloor() function with error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    bcfloor('hoge');
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}

try {
    bcfloor('0.00.1');
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECT--
bcfloor(): Argument #1 ($num) is not well-formed
bcfloor(): Argument #1 ($num) is not well-formed
