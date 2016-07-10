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
Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 2

Deprecated: mcrypt_enc_get_algorithms_name(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 3
Rijndael-128

Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 4

Deprecated: mcrypt_enc_get_algorithms_name(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 5
Rijndael-128

Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 6

Deprecated: mcrypt_enc_get_algorithms_name(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 7
RC2

Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 8

Deprecated: mcrypt_enc_get_algorithms_name(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 9
Blowfish

Deprecated: mcrypt_module_open(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 10

Deprecated: mcrypt_enc_get_algorithms_name(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_enc_get_algorithms_name.php on line 11
DES