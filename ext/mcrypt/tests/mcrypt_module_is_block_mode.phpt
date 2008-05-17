--TEST--
mcrypt_module_is_block_mode
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_is_block_mode(MCRYPT_MODE_CBC));
var_dump(mcrypt_module_is_block_mode(MCRYPT_MODE_ECB));
var_dump(mcrypt_module_is_block_mode(MCRYPT_MODE_STREAM));
var_dump(mcrypt_module_is_block_mode(MCRYPT_MODE_NOFB));
var_dump(mcrypt_module_is_block_mode(MCRYPT_MODE_OFB));
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)