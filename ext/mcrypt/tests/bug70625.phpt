--TEST--
Bug #70625: mcrypt_encrypt() : won't return data when no IV was specified under RC4
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php

$key = 'secretkey';
$ciphertext = mcrypt_encrypt(MCRYPT_ARCFOUR, $key, 'payload', MCRYPT_MODE_STREAM);
var_dump(bin2hex($ciphertext));
$plaintext = mcrypt_decrypt(MCRYPT_ARCFOUR, $key, $ciphertext, MCRYPT_MODE_STREAM);
var_dump($plaintext);

?>
--EXPECTF--
Deprecated: Function mcrypt_encrypt() is deprecated in %s%ebug70625.php on line 4
string(14) "d5c9a57023d0f1"

Deprecated: Function mcrypt_decrypt() is deprecated in %s%ebug70625.php on line 6
string(7) "payload"
