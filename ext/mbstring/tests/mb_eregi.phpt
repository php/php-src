--TEST--
mb_eregi() and invalid arguments
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
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
int(1)
bool(false)
int(1)
