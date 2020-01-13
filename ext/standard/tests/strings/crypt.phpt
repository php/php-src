--TEST--
crypt() function
--FILE--
<?php

$str             = 'rasmuslerdorf';
$salt_md5        = '$1$rasmusle$';
$result_md5      = '$1$rasmusle$rISCgZzpwk3UhDidwXvin0';
$salt_blowfish   = '$2a$07$rasmuslerd............';
$result_blowfish = '$2a$07$rasmuslerd............nIdrcHdxcUxWomQX9j6kvERCFjTg7Ra';

echo (CRYPT_MD5)      ? ((crypt($str, $salt_md5)      === $result_md5)      ? 'MD5' : 'MD5 - ERROR') : 'MD5', "\n";
echo (CRYPT_BLOWFISH) ? ((crypt($str, $salt_blowfish) === $result_blowfish) ? 'BLO' : 'BLO - ERROR') : 'BLO', "\n";

var_dump(crypt($str));

?>
--EXPECTF--
MD5
BLO
string(60) "$2y$10$%r(.{53})%r"
