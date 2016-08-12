--TEST--
Bug #49738 (calling mcrypt after mcrypt_generic_deinit crashes)
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
   $td = mcrypt_module_open(MCRYPT_DES, '', MCRYPT_MODE_ECB, '');
   mcrypt_generic_init($td, 'aaaaaaaa', 'aaaaaaaa');
   mcrypt_generic_deinit($td);
   echo mcrypt_generic($td, 'aaaaaaaa');
?>
--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s%ebug49738.php on line 2

Deprecated: Function mcrypt_generic_init() is deprecated in %s%ebug49738.php on line 3

Deprecated: Function mcrypt_generic_deinit() is deprecated in %s%ebug49738.php on line 4

Deprecated: Function mcrypt_generic() is deprecated in %s%ebug49738.php on line 5

Warning: mcrypt_generic(): Operation disallowed prior to mcrypt_generic_init(). in %sbug49738.php on line 5
