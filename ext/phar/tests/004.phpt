--TEST--
Phar::mapPhar no __HALT_COMPILER();
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
Phar::mapPhar('hio');
?>
--EXPECTF--
Fatal error: Phar::mapPhar(): __HALT_COMPILER(); must be declared in a phar in %s on line %d