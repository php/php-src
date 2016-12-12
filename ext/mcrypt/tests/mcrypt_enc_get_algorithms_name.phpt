--TEST--
mcrypt_enc_get_algorithms_name
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td  = mcrypt_module_open('rijndael-128', '', MCRYPT_MODE_ECB, '');
echo mcrypt_enc_get_algorithms_name($td) . "\n";
$td  = mcrypt_module_open(MCRYPT_RIJNDAEL_128, '', MCRYPT_MODE_ECB, '');
echo mcrypt_enc_get_algorithms_name($td) . "\n";
$td  = mcrypt_module_open(MCRYPT_RC2, '', MCRYPT_MODE_CBC, '');
echo mcrypt_enc_get_algorithms_name($td) . "\n";
$td  = mcrypt_module_open(MCRYPT_BLOWFISH, '', MCRYPT_MODE_CBC, '');
echo mcrypt_enc_get_algorithms_name($td) . "\n";
$td  = mcrypt_module_open('des', '', 'ecb', '');
echo mcrypt_enc_get_algorithms_name($td) . "\n";
--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 2

Deprecated: Function mcrypt_enc_get_algorithms_name() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 3
Rijndael-128

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 4

Deprecated: Function mcrypt_enc_get_algorithms_name() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 5
Rijndael-128

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 6

Deprecated: Function mcrypt_enc_get_algorithms_name() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 7
RC2

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 8

Deprecated: Function mcrypt_enc_get_algorithms_name() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 9
Blowfish

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 10

Deprecated: Function mcrypt_enc_get_algorithms_name() is deprecated in %s%emcrypt_enc_get_algorithms_name.php on line 11
DES