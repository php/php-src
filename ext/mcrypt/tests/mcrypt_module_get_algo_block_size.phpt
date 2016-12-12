--TEST--
mcrypt_module_get_algo_block_size
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
var_dump(mcrypt_module_get_algo_block_size(MCRYPT_RIJNDAEL_256));
var_dump(mcrypt_module_get_algo_block_size(MCRYPT_RIJNDAEL_192));
var_dump(mcrypt_module_get_algo_block_size(MCRYPT_RC2));
var_dump(mcrypt_module_get_algo_block_size(MCRYPT_XTEA));
var_dump(mcrypt_module_get_algo_block_size(MCRYPT_CAST_256));
var_dump(mcrypt_module_get_algo_block_size(MCRYPT_BLOWFISH));
?>
--EXPECTF--
Deprecated: Function mcrypt_module_get_algo_block_size() is deprecated in %s%emcrypt_module_get_algo_block_size.php on line 2
int(32)

Deprecated: Function mcrypt_module_get_algo_block_size() is deprecated in %s%emcrypt_module_get_algo_block_size.php on line 3
int(24)

Deprecated: Function mcrypt_module_get_algo_block_size() is deprecated in %s%emcrypt_module_get_algo_block_size.php on line 4
int(8)

Deprecated: Function mcrypt_module_get_algo_block_size() is deprecated in %s%emcrypt_module_get_algo_block_size.php on line 5
int(8)

Deprecated: Function mcrypt_module_get_algo_block_size() is deprecated in %s%emcrypt_module_get_algo_block_size.php on line 6
int(16)

Deprecated: Function mcrypt_module_get_algo_block_size() is deprecated in %s%emcrypt_module_get_algo_block_size.php on line 7
int(8)
