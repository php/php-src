--TEST--
mcrypt_module_self_test
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_self_test(MCRYPT_RIJNDAEL_128));
var_dump(mcrypt_module_self_test(MCRYPT_RC2));
var_dump(mcrypt_module_self_test(''));
--EXPECTF--
Deprecated: mcrypt_module_self_test(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_self_test.php on line 2
bool(true)

Deprecated: mcrypt_module_self_test(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_self_test.php on line 3
bool(true)

Deprecated: mcrypt_module_self_test(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_self_test.php on line 4
bool(false)
