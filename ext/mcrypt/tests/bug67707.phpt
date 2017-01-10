--TEST--
Bug #67707 IV not needed for ECB encryption mode, but it returns a warning
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open('rijndael-256', '', 'ecb', '');
mcrypt_generic_init($td, 'secret key', NULL);
?>
--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s on line %d

Deprecated: Function mcrypt_generic_init() is deprecated in %s on line %d
