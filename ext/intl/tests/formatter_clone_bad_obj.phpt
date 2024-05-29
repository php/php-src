--TEST--
Cloning uninitialized NumberFormatter
--EXTENSIONS--
intl
--FILE--
<?php

class A extends NumberFormatter {
    function __construct() {}
}

$a = new A;
try {
    $b = clone $a;
    var_dump($b);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Cannot clone uninitialized NumberFormatter
