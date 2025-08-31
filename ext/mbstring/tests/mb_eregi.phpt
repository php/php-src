--TEST--
mb_eregi() and invalid arguments
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_eregi")) print "skip mb_eregi() not available";
?>
--FILE--
<?php

mb_regex_encoding('utf-8');

var_dump(mb_eregi('z', 'XYZ'));
var_dump(mb_eregi('xyzp', 'XYZ'));
var_dump(mb_eregi('ö', 'Öäü'));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
