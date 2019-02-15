--TEST--
Phar::mapPhar truncated manifest/improper params
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
Phar::mapPhar(5, 'hio', 'hi');

Phar::mapPhar();
} catch (Exception $e) {
	echo $e->getMessage();
}
__HALT_COMPILER(); ?>
--EXPECTF--
Warning: Phar::mapPhar() expects at most 2 parameters, 3 given in %s002.php on line %d
internal corruption of phar "%s002.php" (truncated manifest at manifest length)
