--TEST--
strstr() - New parameter: before_needle
--FILE--
<?php

$email  = 'aexample.com';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', true));
$email  = 'a@example.com';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', true));
$email  = 'asdfasdfas@e';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', true));
$email  = '@';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', true));
$email  = 'eE@fF';
var_dump(strstr($email, 'e'));
var_dump(strstr($email, 'e', true));
var_dump(strstr($email, 'E'));
var_dump(strstr($email, 'E', true));

var_dump(strstr('', ' '));

?>
--EXPECT--
bool(false)
bool(false)
string(12) "@example.com"
string(1) "a"
string(2) "@e"
string(10) "asdfasdfas"
string(1) "@"
string(0) ""
string(5) "eE@fF"
string(0) ""
string(4) "E@fF"
string(1) "e"
bool(false)
