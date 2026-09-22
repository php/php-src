--TEST--
bcceil() function with error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    bcceil('hoge');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    bcceil('0.00.1');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: bcceil(): Argument #1 ($num) is not well-formed
ValueError: bcceil(): Argument #1 ($num) is not well-formed
