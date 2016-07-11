--TEST--
mcrypt_module_open
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_open(MCRYPT_RIJNDAEL_256, '', MCRYPT_MODE_CBC, ''));
mcrypt_module_open('', '', '', '');

--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_module_open.php on line 2
resource(%d) of type (mcrypt)

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_module_open.php on line 3

Warning: mcrypt_module_open(): Could not open encryption module in %s on line %d

