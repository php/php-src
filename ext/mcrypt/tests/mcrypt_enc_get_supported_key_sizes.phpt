--TEST--
mcrypt_enc_get_supported_key_sizes
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td  = mcrypt_module_open('rijndael-256', '', 'ecb', '');
$var = mcrypt_enc_get_supported_key_sizes($td);
var_dump($var);
--EXPECT--
array(3) {
  [0]=>
  int(16)
  [1]=>
  int(24)
  [2]=>
  int(32)
}