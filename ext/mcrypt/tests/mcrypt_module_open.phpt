--TEST--
mcrypt_module_open
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_open(MCRYPT_RIJNDAEL_256, '', MCRYPT_MODE_CBC, ''));
mcrypt_module_open('', '', '', '');

--EXPECTF--
resource(%d) of type (mcrypt)

Warning: mcrypt_module_open(): Could not open encryption module in %s on line %d

