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

var_dump(crypt($str));
var_dump(crypt());
var_dump(crypt("", "", ""));

?>
--EXPECTF--
STD
EXT
MD5
BLO
string(%d) "%s"

Warning: crypt() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: crypt() expects at most 2 parameters, 3 given in %s on line %d
NULL
