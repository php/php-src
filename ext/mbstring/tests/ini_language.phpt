--TEST--
mbstring.language bug
--EXTENSIONS--
mbstring
--INI--
internal_encoding=Shift_JIS
mbstring.language=Japanese
--FILE--
<?php
var_dump(ini_get('internal_encoding'));
var_dump(mb_internal_encoding());
?>
--EXPECTF--
string(9) "Shift_JIS"

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
string(4) "SJIS"
