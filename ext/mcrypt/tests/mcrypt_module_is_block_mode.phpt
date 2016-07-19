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
--EXPECTF--
Deprecated: Function mcrypt_module_is_block_mode() is deprecated in %s%emcrypt_module_is_block_mode.php on line 2
bool(true)

Deprecated: Function mcrypt_module_is_block_mode() is deprecated in %s%emcrypt_module_is_block_mode.php on line 3
bool(true)

Deprecated: Function mcrypt_module_is_block_mode() is deprecated in %s%emcrypt_module_is_block_mode.php on line 4
bool(false)

Deprecated: Function mcrypt_module_is_block_mode() is deprecated in %s%emcrypt_module_is_block_mode.php on line 5
bool(false)

Deprecated: Function mcrypt_module_is_block_mode() is deprecated in %s%emcrypt_module_is_block_mode.php on line 6
bool(false)