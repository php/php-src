--TEST--
Test crypt_gensalt
--SKIPIF--
<?php if (!function_exists('crypt_gensalt')) die('skip needs external libxcrypt >= 4.4'); ?>
--FILE--
<?php
var_dump(crypt_gensalt(CRYPT_PREFIX_STD_DES));
var_dump(crypt_gensalt(CRYPT_PREFIX_EXT_DES));
var_dump(crypt_gensalt(CRYPT_PREFIX_MD5));
var_dump(crypt_gensalt(CRYPT_PREFIX_BLOWFISH));
var_dump(crypt_gensalt(CRYPT_PREFIX_SHA256));
var_dump(crypt_gensalt(CRYPT_PREFIX_SHA512));
var_dump(crypt_gensalt(CRYPT_PREFIX_SCRYPT));
var_dump(crypt_gensalt(CRYPT_PREFIX_GOST_YESCRYPT));
var_dump(crypt_gensalt(CRYPT_PREFIX_YESCRYPT));

?>
--EXPECTF--
string(2) "%s"
string(9) "_%s"
string(11) "$1$%s"
string(29) "$2y$%s"
string(19) "$5$%s"
string(19) "$6$%s"
string(36) "$7$%s"
string(30) "$gy$%s"
string(29) "$y$j%s"
