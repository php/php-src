--TEST--
Test crypt() with invalid salts
--FILE--
<?php
var_dump(crypt("test", "$#"));
var_dump(crypt("test", "$5zd$01"));
var_dump(crypt("test", "$2"));
var_dump(crypt("test", "$1zd$01"));
var_dump(crypt("test", "$2$"));
?>
--EXPECT--
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
string(2) "*0"
