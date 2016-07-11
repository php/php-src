--TEST--
Bug #46010 (warnings incorrectly generated for iv in ecb mode)
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php

$key = "012345678901234567890123";
var_dump(bin2hex(mcrypt_encrypt(MCRYPT_TRIPLEDES, $key, "data", MCRYPT_MODE_ECB)));
var_dump(bin2hex(mcrypt_encrypt(MCRYPT_TRIPLEDES, $key, "data", MCRYPT_MODE_ECB, "a")));
var_dump(bin2hex(mcrypt_encrypt(MCRYPT_TRIPLEDES, $key, "data", MCRYPT_MODE_ECB, "12345678")));

?>
--EXPECTF--
Deprecated: Function mcrypt_encrypt() is deprecated in %s%ebug46010.php on line 4
string(16) "f7a2ce11d4002294"

Deprecated: Function mcrypt_encrypt() is deprecated in %s%ebug46010.php on line 5
string(16) "f7a2ce11d4002294"

Deprecated: Function mcrypt_encrypt() is deprecated in %s%ebug46010.php on line 6
string(16) "f7a2ce11d4002294"
