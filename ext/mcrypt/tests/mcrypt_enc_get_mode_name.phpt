--TEST--
mcrypt_enc_get_modes_name
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$td  = mcrypt_module_open('rijndael-128', '', MCRYPT_MODE_ECB, '');
echo mcrypt_enc_get_modes_name($td) . "\n";
$td  = mcrypt_module_open(MCRYPT_RIJNDAEL_128, '', MCRYPT_MODE_CBC, '');
echo mcrypt_enc_get_modes_name($td) . "\n";
$td  = mcrypt_module_open(MCRYPT_WAKE, '', MCRYPT_MODE_STREAM, '');
echo mcrypt_enc_get_modes_name($td) . "\n";
$td  = mcrypt_module_open(MCRYPT_BLOWFISH, '', MCRYPT_MODE_OFB, '');
echo mcrypt_enc_get_modes_name($td) . "\n";
$td  = mcrypt_module_open('des', '', 'ecb', '');
echo mcrypt_enc_get_modes_name($td) . "\n";
$td  = mcrypt_module_open('des', '', 'cbc', '');
echo mcrypt_enc_get_modes_name($td) . "\n";
--EXPECTF--
Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 2

Deprecated: Function mcrypt_enc_get_modes_name() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 3
ECB

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 4

Deprecated: Function mcrypt_enc_get_modes_name() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 5
CBC

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 6

Deprecated: Function mcrypt_enc_get_modes_name() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 7
STREAM

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 8

Deprecated: Function mcrypt_enc_get_modes_name() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 9
OFB

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 10

Deprecated: Function mcrypt_enc_get_modes_name() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 11
ECB

Deprecated: Function mcrypt_module_open() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 12

Deprecated: Function mcrypt_enc_get_modes_name() is deprecated in %s%emcrypt_enc_get_mode_name.php on line 13
CBC
