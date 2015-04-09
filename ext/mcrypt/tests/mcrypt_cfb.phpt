--TEST--
mcrypt_cfb
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$key      = "0123456789012345";
$secret   = "PHP Testfest 2008";
$cipher   = MCRYPT_RIJNDAEL_128;

$iv       = mcrypt_create_iv(mcrypt_get_iv_size($cipher, MCRYPT_MODE_CFB), MCRYPT_RAND);
$enc_data = mcrypt_cfb($cipher, $key, $secret, MCRYPT_ENCRYPT, $iv);

// we have to trim as AES rounds the blocks and decrypt doesnt detect that
echo trim(mcrypt_cfb($cipher, $key, $enc_data, MCRYPT_DECRYPT, $iv)) . "\n";

// a warning must be issued if we don't use a IV on a AES cipher, that usually requires an IV
var_dump(mcrypt_cfb($cipher, $key, $enc_data, MCRYPT_DECRYPT));

--EXPECTF--

Deprecated: Function mcrypt_cfb() is deprecated in %s on line %d

Deprecated: Function mcrypt_cfb() is deprecated in %s on line %d
PHP Testfest 2008

Deprecated: Function mcrypt_cfb() is deprecated in %s on line %d

Warning: mcrypt_cfb(): Encryption mode requires an initialization vector of size 16 in %s on line %d
bool(false)
