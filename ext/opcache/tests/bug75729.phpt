--TEST--
Bug #75729: opcache segfault when installing Bitrix
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded('mbstring')) die('skip mbstring not loaded'); ?>
--INI--
opcache.enable_cli=1
mbstring.func_overload=2
--FILE--
<?php

var_dump(strpos("foo", "o"));
var_dump(substr("foo", 1));
var_dump(substr("foo", 1, 1));

?>
--EXPECT--
Deprecated: The mbstring.func_overload directive is deprecated in Unknown on line 0
int(1)
string(2) "oo"
string(1) "o"
