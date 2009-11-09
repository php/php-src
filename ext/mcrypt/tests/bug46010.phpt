--TEST--
Bug #46010 (warnings incorrectly generated for iv in ecb mode)
--FILE--
<?php

var_dump(bin2hex(mcrypt_encrypt(MCRYPT_TRIPLEDES, "key", "data", MCRYPT_MODE_ECB)));
var_dump(bin2hex(mcrypt_encrypt(MCRYPT_TRIPLEDES, "key", "data", MCRYPT_MODE_ECB, "a")));
var_dump(bin2hex(mcrypt_encrypt(MCRYPT_TRIPLEDES, "key", "data", MCRYPT_MODE_ECB, "12345678")));

?>
--EXPECTF--
string(16) "372eeb4a524b8d31"
string(16) "372eeb4a524b8d31"
string(16) "372eeb4a524b8d31"
