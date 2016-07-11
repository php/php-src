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
Deprecated: Function mcrypt_module_self_test() is deprecated in %s%emcrypt_module_self_test.php on line 2
bool(true)

Deprecated: Function mcrypt_module_self_test() is deprecated in %s%emcrypt_module_self_test.php on line 3
bool(true)

Deprecated: Function mcrypt_module_self_test() is deprecated in %s%emcrypt_module_self_test.php on line 4
bool(false)
