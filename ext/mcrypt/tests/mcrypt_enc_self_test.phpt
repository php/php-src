--TEST--
mcrypt_enc_self_test
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open(MCRYPT_RIJNDAEL_256, '', MCRYPT_MODE_CBC, '');
var_dump(mcrypt_enc_self_test($td));
--EXPECTF--
Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_self_test.php on line 2

Deprecated: mcrypt_enc_self_test(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_self_test.php on line 3
int(0)