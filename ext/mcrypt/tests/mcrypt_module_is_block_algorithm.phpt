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
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)