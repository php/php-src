--TEST--
Phar::mapPhar no __HALT_COMPILER();
--EXTENSIONS--
phar
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
PharException: __HALT_COMPILER(); must be declared in a phar
