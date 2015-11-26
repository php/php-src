--TEST--
Bug #52931 (strripos not overloaded with function overloading enabled)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.func_overload = 7
mbstring.internal_encoding = utf-8
--FILE--
<?php

$string = '<body>Umlauttest öüä</body>';

var_dump(strlen($string));
var_dump(mb_strlen($string));

var_dump(strripos($string, '</body>'));
var_dump(mb_strripos($string, '</body>'));

?>
--EXPECTF--
int(27)
int(27)
int(20)
int(20)
