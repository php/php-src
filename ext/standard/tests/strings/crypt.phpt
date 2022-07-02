--TEST--
crypt() function
--FILE--
<?php

$str = 'rasmuslerdorf';
$salt1 = 'rl';
$res_1 = 'rl.3StKT.4T8M';
$salt2 = '_J9..rasm';
$res_2 = '_J9..rasmBYk8r9AiWNc';
$salt3 = '$1$rasmusle$';
$res_3 = '$1$rasmusle$rISCgZzpwk3UhDidwXvin0';
$salt4 = '$2a$07$rasmuslerd............';
$res_4 = '$2a$07$rasmuslerd............nIdrcHdxcUxWomQX9j6kvERCFjTg7Ra';

echo (CRYPT_STD_DES)  ? ((crypt($str, $salt1) === $res_1) ? 'STD' : 'STD - ERROR') : 'STD', "\n";
echo (CRYPT_EXT_DES)  ? ((crypt($str, $salt2) === $res_2) ? 'EXT' : 'EXT - ERROR') : 'EXT', "\n";
echo (CRYPT_MD5)      ? ((crypt($str, $salt3) === $res_3) ? 'MD5' : 'MD5 - ERROR') : 'MD5', "\n";
echo (CRYPT_BLOWFISH) ? ((crypt($str, $salt4) === $res_4) ? 'BLO' : 'BLO - ERROR') : 'BLO', "\n";

try {
    var_dump(crypt($str));
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
STD
EXT
MD5
BLO
crypt() expects exactly 2 arguments, 1 given
