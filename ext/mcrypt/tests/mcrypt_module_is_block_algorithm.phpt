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
Deprecated: mcrypt_module_is_block_algorithm(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_is_block_algorithm.php on line 2
bool(true)

Deprecated: mcrypt_module_is_block_algorithm(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_is_block_algorithm.php on line 3
bool(true)

Deprecated: mcrypt_module_is_block_algorithm(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_is_block_algorithm.php on line 4
bool(false)

Deprecated: mcrypt_module_is_block_algorithm(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_module_is_block_algorithm.php on line 5
bool(true)