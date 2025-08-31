--TEST--
bcceil() function with error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    bcceil('hoge');
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}

try {
    bcceil('0.00.1');
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECT--
bcceil(): Argument #1 ($num) is not well-formed
bcceil(): Argument #1 ($num) is not well-formed
