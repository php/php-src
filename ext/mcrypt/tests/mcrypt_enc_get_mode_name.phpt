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
--EXPECT--
ECB
CBC
STREAM
OFB
ECB
CBC
