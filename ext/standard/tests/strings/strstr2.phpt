--TEST--
strstr() - New parameter: before_needle
--FILE--
<?php

$email  = 'aexample.com';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', 1));
$email  = 'a@example.com';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', 1));
$email  = 'asdfasdfas@e';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', 1));
$email  = '@';
var_dump(strstr($email, '@'));
var_dump(strstr($email, '@', 1));
$email  = 'eE@fF';
var_dump(strstr($email, 'e'));
var_dump(strstr($email, 'e', 1));
var_dump(strstr($email, 'E'));
var_dump(strstr($email, 'E', 1));

var_dump(strstr('', ' ', ''));

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
