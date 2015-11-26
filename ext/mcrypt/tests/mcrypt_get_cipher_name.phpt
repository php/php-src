--TEST--
mcrypt_get_cipher_name
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
echo mcrypt_get_cipher_name(MCRYPT_RIJNDAEL_256) . "\n";
echo mcrypt_get_cipher_name(MCRYPT_RC2) . "\n";
echo mcrypt_get_cipher_name(MCRYPT_ARCFOUR) . "\n";
echo mcrypt_get_cipher_name(MCRYPT_WAKE) . "\n";
--EXPECT--
Rijndael-256
RC2
RC4
WAKE
