--TEST--
mcrypt_ofb
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$key      = "0123456789012345";
$secret   = "PHP Testfest 2008";
$cipher   = MCRYPT_RIJNDAEL_128;

$iv       = mcrypt_create_iv(mcrypt_get_iv_size($cipher, MCRYPT_MODE_OFB), MCRYPT_RAND);
$enc_data = mcrypt_decrypt($cipher, $key, $secret, MCRYPT_MODE_OFB, $iv);

// we have to trim as AES rounds the blocks and decrypt doesnt detect that
echo trim(mcrypt_decrypt($cipher, $key, $enc_data, MCRYPT_MODE_OFB, $iv)) . "\n";

// a warning must be issued if we don't use a IV on a AES cipher, that usually requires an IV
mcrypt_decrypt($cipher, $key, $enc_data, MCRYPT_MODE_OFB);

--EXPECTF--
PHP Testfest 2008

Warning: mcrypt_decrypt(): Encryption mode requires an initialization vector of size 16 in %s on line %d
