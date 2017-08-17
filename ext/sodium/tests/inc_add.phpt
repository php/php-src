--TEST--
increment and add edge cases
--SKIPIF--
<?php if (!extension_loaded("sodium")) die("skip"); ?>
--FILE--
<?php

$notStr = 123;
try {
	sodium_increment($notStr);
} catch (TypeError $e) {
	echo $e->getMessage(), "\n";
}

$str = "abc";
$str2 = $str;
sodium_increment($str);
var_dump($str, $str2);

$str = "ab" . ($x = "c");
$str2 = $str;
sodium_increment($str);
var_dump($str, $str2);

$addStr = "\2\0\0";

$notStr = 123;
try {
	sodium_add($notStr, $addStr);
} catch (TypeError $e) {
	echo $e->getMessage(), "\n";
}

$str = "abc";
$str2 = $str;
sodium_add($str, $addStr);
var_dump($str, $str2);

$str = "ab" . ($x = "c");
$str2 = $str;
sodium_add($str, $addStr);
var_dump($str, $str2);

?>
--EXPECT--
sodium_increment() expects parameter 1 to be string, integer given
string(3) "bbc"
string(3) "abc"
string(3) "bbc"
string(3) "abc"
sodium_add() expects parameter 1 to be string, integer given
string(3) "cbc"
string(3) "abc"
string(3) "cbc"
string(3) "abc"
