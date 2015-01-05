--TEST--
mcrypt_enc_is_block_algorithm
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open(MCRYPT_RIJNDAEL_256, '', MCRYPT_MODE_CBC, '');
var_dump(mcrypt_enc_is_block_algorithm($td));
$td = mcrypt_module_open(MCRYPT_3DES, '', MCRYPT_MODE_CBC, '');
var_dump(mcrypt_enc_is_block_algorithm($td));
$td = mcrypt_module_open(MCRYPT_WAKE, '', MCRYPT_MODE_STREAM, '');
var_dump(mcrypt_enc_is_block_algorithm($td));
--EXPECT--
bool(true)
bool(true)
bool(false)
