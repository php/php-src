--TEST--
Phar::mapPhar truncated manifest (none)
--EXTENSIONS--
phar
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
__HALT_COMPILER(); ?>()
--EXPECTF--
PharException: internal corruption of phar "%s" (truncated manifest at manifest length)
