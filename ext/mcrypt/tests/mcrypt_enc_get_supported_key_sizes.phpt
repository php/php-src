--TEST--
mcrypt_enc_get_supported_key_sizes
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td  = mcrypt_module_open('rijndael-256', '', 'ecb', '');
$var = mcrypt_enc_get_supported_key_sizes($td);
var_dump($var);
--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_supported_key_sizes.php on line 2

Deprecated: Function mcrypt_enc_get_supported_key_sizes() is deprecated in %s%emcrypt_enc_get_supported_key_sizes.php on line 3
array(3) {
  [0]=>
  int(16)
  [1]=>
  int(24)
  [2]=>
  int(32)
}