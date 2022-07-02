--TEST--
Test DES with invalid fallback
--FILE--
<?php

var_dump(crypt("test", "$#"));
var_dump(crypt("test", "$5zd$01"));

?>
--EXPECT--
string(2) "*0"
string(2) "*0"
