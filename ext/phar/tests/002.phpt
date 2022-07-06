--TEST--
Phar::mapPhar truncated manifest/improper params
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
    Phar::mapPhar(5, 'hio', 'hi');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    Phar::mapPhar();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
__HALT_COMPILER(); ?>
--EXPECTF--
Phar::mapPhar() expects at most 2 arguments, 3 given
internal corruption of phar "%s002.php" (truncated manifest at manifest length)
