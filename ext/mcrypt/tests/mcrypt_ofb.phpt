--TEST--
mcrypt_ofb
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$key      = "0123456789012345";
$secret   = "PHP Testfest 2008";
$cipher   = MCRYPT_RIJNDAEL_128;

$iv       = mcrypt_create_iv(mcrypt_get_iv_size($cipher, MCRYPT_MODE_ECB), MCRYPT_RAND);
$enc_data = mcrypt_ofb($cipher, $key, $secret, MCRYPT_ENCRYPT, $iv);

// we have to trim as AES rounds the blocks and decrypt doesnt detect that
echo trim(mcrypt_ofb($cipher, $key, $enc_data, MCRYPT_DECRYPT, $iv)) . "\n";

// a warning must be issued if we don't use a IV on a AES cipher, that usually requires an IV
mcrypt_ofb($cipher, $key, $enc_data, MCRYPT_DECRYPT, $iv);

--EXPECTF--

Deprecated: Function mcrypt_ofb() is deprecated in %s on line %d

Deprecated: Function mcrypt_ofb() is deprecated in %s on line %d
PHP Testfest 2008

Deprecated: Function mcrypt_ofb() is deprecated in %s on line %d
