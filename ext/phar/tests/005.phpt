--TEST--
Phar::mapPhar truncated manifest (none)
--EXTENSIONS--
phar
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e->getMessage();
}
__HALT_COMPILER(); ?>()
--EXPECTF--
internal corruption of phar "%s" (truncated manifest at manifest length)
