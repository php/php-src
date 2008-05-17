--TEST--
mcrypt_create_iv
--SKIPIF--
<?php if (!extension_loaded("mcrypt")) print "skip"; ?>
--FILE--
<?php
$iv1 = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_ECB), MCRYPT_RAND);
$iv2 = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_192, MCRYPT_MODE_ECB), MCRYPT_DEV_URANDOM);
$iv3 = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB), MCRYPT_DEV_RANDOM);

echo strlen($iv1) . "\n";
echo strlen($iv2) . "\n";
echo strlen($iv3) . "\n";
--EXPECT--
16
24
32
