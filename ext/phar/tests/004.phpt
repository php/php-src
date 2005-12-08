--TEST--
PHP_Archive::mapPhar improper parameters
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
PHP_Archive::mapPhar(5, 'hio', false);
?>
--EXPECTF--
Fatal error: PHP_Archive::mapPhar(): __HALT_COMPILER(); must be declared in a phar in %s on line %d