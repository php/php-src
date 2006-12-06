--TEST--
crypt() function
--SKIPIF--
<?php
if (!function_exists('crypt')) {
	die("SKIP crypt() is not available");
}
?> 
--FILE--
<?php

$str = b'rasmuslerdorf';
$salt1 = b'rl';
$res_1 = b'rl.3StKT.4T8M';
$salt2 = b'_J9..rasm';
$res_2 = b'_J9..rasmBYk8r9AiWNc';
$salt3 = b'$1$rasmusle$';
$res_3 = b'$1$rasmusle$rISCgZzpwk3UhDidwXvin0';
$salt4 = b'$2a$07$rasmuslerd............';
$res_4 = b'$2a$07$rasmuslerd............nIdrcHdxcUxWomQX9j6kvERCFjTg7Ra';

echo (CRYPT_STD_DES)  ? ((crypt($str, $salt1) === $res_1) ? 'STD' : 'STD - ERROR') : 'STD', "\n";
echo (CRYPT_EXT_DES)  ? ((crypt($str, $salt2) === $res_2) ? 'EXT' : 'EXT - ERROR') : 'EXT', "\n";
echo (CRYPT_MD5)      ? ((crypt($str, $salt3) === $res_3) ? 'MD5' : 'MD5 - ERROR') : 'MD5', "\n";
echo (CRYPT_BLOWFISH) ? ((crypt($str, $salt4) === $res_4) ? 'BLO' : 'BLO - ERROR') : 'BLO', "\n";

?>
--EXPECT--
STD
EXT
MD5
BLO
