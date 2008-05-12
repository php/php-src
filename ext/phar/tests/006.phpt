--TEST--
Phar::mapPhar truncated manifest (manifest length truncated)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e->getMessage();
}
__HALT_COMPILER(); ?>
--EXPECTF--
internal corruption of phar "%s" (truncated manifest at manifest length)
