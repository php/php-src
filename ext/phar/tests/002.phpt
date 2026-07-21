--TEST--
Phar::mapPhar truncated manifest/improper params
--EXTENSIONS--
phar
--FILE--
<?php
try {
    Phar::mapPhar(5, 'hio', 'hi');
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    Phar::mapPhar();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
__HALT_COMPILER(); ?>
--EXPECTF--
ArgumentCountError: Phar::mapPhar() expects at most 2 arguments, 3 given
PharException: internal corruption of phar "%s002.php" (truncated manifest at manifest length)
