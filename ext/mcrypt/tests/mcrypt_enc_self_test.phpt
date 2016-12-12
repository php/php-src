--TEST--
mcrypt_enc_self_test
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open(MCRYPT_RIJNDAEL_256, '', MCRYPT_MODE_CBC, '');
var_dump(mcrypt_enc_self_test($td));
--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_self_test.php on line 2

Deprecated: Function mcrypt_enc_self_test() is deprecated in %s%emcrypt_enc_self_test.php on line 3
int(0)