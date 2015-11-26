--TEST--
mcrypt_enc_get_block_size
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td = mcrypt_module_open(MCRYPT_RIJNDAEL_256, '', MCRYPT_MODE_CBC, '');
var_dump(mcrypt_enc_get_block_size($td));
$td = mcrypt_module_open(MCRYPT_3DES, '', MCRYPT_MODE_CBC, '');
var_dump(mcrypt_enc_get_block_size($td));
$td = mcrypt_module_open(MCRYPT_WAKE, '', MCRYPT_MODE_STREAM, '');
var_dump(mcrypt_enc_get_block_size($td));
--EXPECT--
int(32)
int(8)
int(1)
