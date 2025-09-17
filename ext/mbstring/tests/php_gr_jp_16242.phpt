--TEST--
php-users@php.gr.jp #16242
--EXTENSIONS--
mbstring
--INI--
mbstring.language=Japanese
internal_encoding=UTF-8
--FILE--
<?php

var_dump(ini_get('mbstring.language'));
var_dump(ini_get('internal_encoding'));
var_dump(mb_internal_encoding());

?>
--EXPECTF--
string(8) "Japanese"
string(5) "UTF-8"

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
string(5) "UTF-8"
