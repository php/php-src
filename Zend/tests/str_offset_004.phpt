--TEST--
string offset 004
--FILE--
<?php
// Test assignments using (positive and negative) string offsets

$str = "abcdefghijklmno";
$i = 3;
$j = -4;

$str[2] = 'C';
var_dump($str);

$str[$i] = 'Z';
var_dump($str);

$str[-5] = 'P';
var_dump($str);

$str[$j] = 'Q';
var_dump($str);

$str[-20] = 'Y';
var_dump($str);

$str[-strlen($str)] = strtoupper($str[0]); /* An exotic ucfirst() ;) */
var_dump($str);

$str[20] = 'N';
var_dump($str);

$str[-2] = 'UFO';
var_dump($str);

$str[-$i] = $str[$j*2];
var_dump($str);
?>
--EXPECTF--
string(15) "abCdefghijklmno"
string(15) "abCZefghijklmno"
string(15) "abCZefghijPlmno"
string(15) "abCZefghijPQmno"

Warning: Illegal string offset:  -20 in %sstr_offset_004.php on line %d
string(15) "abCZefghijPQmno"
string(15) "AbCZefghijPQmno"
string(21) "AbCZefghijPQmno     N"
string(21) "AbCZefghijPQmno    UN"
string(21) "AbCZefghijPQmno   nUN"
