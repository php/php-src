--TEST--
str_is_ascii() function basic functionality
--FILE--
<?php
var_dump(str_is_ascii(''));
var_dump(str_is_ascii('ABC 123'));
var_dump(str_is_ascii('Iñtërnâtiônàlizætiøn'));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
