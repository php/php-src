--TEST--
PHP_Archive::mapPhar truncated manifest (none)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
PHP_Archive::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>
--EXPECTF--
Fatal error: PHP_Archive::mapPhar(): internal corruption of phar "%s" (truncated manifest) in %s on line %d