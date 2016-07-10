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
--EXPECTF--
Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_block_size.php on line 2

Deprecated: mcrypt_enc_get_block_size(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_block_size.php on line 3
int(32)

Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_block_size.php on line 4

Deprecated: mcrypt_enc_get_block_size(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_block_size.php on line 5
int(8)

Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_block_size.php on line 6

Deprecated: mcrypt_enc_get_block_size(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_block_size.php on line 7
int(1)
