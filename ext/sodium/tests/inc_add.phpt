--TEST--
increment and add edge cases
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php

$notStr = 123;
try {
    sodium_increment($notStr);
} catch (SodiumException $e) {
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
} catch (SodiumException $e) {
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
a PHP string is required
string(3) "bbc"
string(3) "abc"
string(3) "bbc"
string(3) "abc"
PHP strings are required
string(3) "cbc"
string(3) "abc"
string(3) "cbc"
string(3) "abc"
