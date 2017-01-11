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
