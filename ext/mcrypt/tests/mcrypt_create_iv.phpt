--TEST--
mcrypt_create_iv
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$iv1 = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB), MCRYPT_RAND);
$iv2 = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB), MCRYPT_DEV_URANDOM);
$iv3 = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB), MCRYPT_DEV_RANDOM);

echo strlen($iv1) . "\n";
echo strlen($iv2) . "\n";
echo strlen($iv3) . "\n";
--EXPECTF--
Deprecated: mcrypt_get_iv_size(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_create_iv.php on line 2

Deprecated: mcrypt_create_iv(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_create_iv.php on line 2

Deprecated: mcrypt_get_iv_size(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_create_iv.php on line 3

Deprecated: mcrypt_create_iv(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_create_iv.php on line 3

Deprecated: mcrypt_get_iv_size(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_create_iv.php on line 4

Deprecated: mcrypt_create_iv(): The mcrypt extension is deprecated and will be removed in the future: use openssl instead in %s%emcrypt_create_iv.php on line 4
16
16
16
