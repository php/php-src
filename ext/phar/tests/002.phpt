--TEST--
PHP_Archive::mapPhar truncated manifest/improper params
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
Phar::mapPhar();
Phar::mapPhar(5);
Phar::mapPhar(5, 5);
Phar::mapPhar(5, 'hio');
Phar::mapPhar(5, 'hio', 'hi');
__HALT_COMPILER(); ?>
--EXPECTF--
Warning: Phar::mapPhar() expects at least 2 parameters, 0 given in %s on line %d

Warning: Phar::mapPhar() expects at least 2 parameters, 1 given in %s on line %d

Fatal error: Phar::mapPhar(): internal corruption of phar "%s" (truncated manifest) in %s on line %d