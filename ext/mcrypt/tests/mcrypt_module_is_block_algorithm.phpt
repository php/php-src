--TEST--
mcrypt_module_is_block_algorithm
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_is_block_algorithm(MCRYPT_RIJNDAEL_128));
var_dump(mcrypt_module_is_block_algorithm(MCRYPT_DES));
var_dump(mcrypt_module_is_block_algorithm(MCRYPT_WAKE));
var_dump(mcrypt_module_is_block_algorithm(MCRYPT_XTEA));
--EXPECTF--
Deprecated: Function mcrypt_module_is_block_algorithm() is deprecated in %s%emcrypt_module_is_block_algorithm.php on line 2
bool(true)

Deprecated: Function mcrypt_module_is_block_algorithm() is deprecated in %s%emcrypt_module_is_block_algorithm.php on line 3
bool(true)

Deprecated: Function mcrypt_module_is_block_algorithm() is deprecated in %s%emcrypt_module_is_block_algorithm.php on line 4
bool(false)

Deprecated: Function mcrypt_module_is_block_algorithm() is deprecated in %s%emcrypt_module_is_block_algorithm.php on line 5
bool(true)