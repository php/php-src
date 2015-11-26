--TEST--
mcrypt_module_self_test
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_self_test(MCRYPT_RIJNDAEL_128));
var_dump(mcrypt_module_self_test(MCRYPT_RC2));
var_dump(mcrypt_module_self_test(''));
--EXPECT--
bool(true)
bool(true)
bool(false)
