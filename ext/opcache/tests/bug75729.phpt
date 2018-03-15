--TEST--
Bug #75729: opcache segfault when installing Bitrix
--SKIPIF--
<?php if (!extension_loaded('mbstring')) die('skip mbstring not loaded'); ?>
--INI--
opcache.enable_cli=1
mbstring.func_overload=2
--FILE--
<?php

var_dump(strpos("foo", "o"));

?>
--EXPECT--
Deprecated: The mbstring.func_overload directive is deprecated in Unknown on line 0
int(1)
