--TEST--
Unix locale names are rejected on Windows, except for some special cases
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip this test is for Windows platforms only');
?>
--FILE--
<?php
var_dump(setlocale(LC_ALL, 'de_DE'));
var_dump(setlocale(LC_ALL, 'de_DE.UTF-8'));
// the following are supposed to be accepted
var_dump(setlocale(LC_ALL, 'uk_UK'));
var_dump(setlocale(LC_ALL, 'uk_US'));
var_dump(setlocale(LC_ALL, 'us_UK'));
var_dump(setlocale(LC_ALL, 'us_US'));
?>
--EXPECT--
bool(false)
bool(false)
string(27) "English_United Kingdom.1252"
string(26) "English_United States.1252"
string(27) "English_United Kingdom.1252"
string(26) "English_United States.1252"
