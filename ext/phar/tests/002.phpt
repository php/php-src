--TEST--
PHP_Archive::mapPhar improper parameters
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
PHP_Archive::mapPhar();
PHP_Archive::mapPhar(5);
PHP_Archive::mapPhar(5, 5);
PHP_Archive::mapPhar(5, 'hio');
PHP_Archive::mapPhar(5, 'hio', 'hi');
PHP_Archive::mapPhar(5, 'hio', true, 5, 5);
__HALT_COMPILER();
?>
--EXPECTF--
Warning: PHP_Archive::mapPhar() expects at least 3 parameters, 0 given in %s on line %d

Warning: PHP_Archive::mapPhar() expects at least 3 parameters, 1 given in %s on line %d

Warning: PHP_Archive::mapPhar() expects at least 3 parameters, 2 given in %s on line %d

Warning: PHP_Archive::mapPhar() expects at least 3 parameters, 2 given in %s on line %d

Fatal error: PHP_Archive::mapPhar(): internal corruption of phar "%s" (truncated manifest) in %s on line %d