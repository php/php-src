--TEST--
Bug #68644 strlen incorrect : mbstring + func_overload=2 + UTF-8 + Opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
mbstring.func_overload=2
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache') || !extension_loaded("mbstring")) die("skip"); ?>
--FILE--
<?php
var_dump(strlen("中国, 北京"));
var_dump(mb_strlen("中国, 北京"));
?>
--EXPECT--
Deprecated: The mbstring.func_overload directive is deprecated in Unknown on line 0
int(6)
int(6)
