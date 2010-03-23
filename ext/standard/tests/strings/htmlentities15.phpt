--TEST--
htmlentities() test 15 (setlocale / KOI8-R)
--INI--
output_handler=
default_charset=
mbstring.internal_encoding=pass
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "ru_RU.koi8r", "ru_RU.KOI8-R");
if (!$result || preg_match('/koi8/i', setlocale(LC_CTYPE, 0)) == 0) {
	die("skip setlocale() failed\n");
}
?>
--FILE--
<?php
setlocale(LC_CTYPE, "ru_RU.koi8r", "ru_RU.KOI8-R");
$str = "роскошный";
var_dump($str, htmlentities($str, ENT_QUOTES, ''));
?>
--EXPECT--
string(9) "роскошный"
string(63) "&#1088;&#1086;&#1089;&#1082;&#1086;&#1096;&#1085;&#1099;&#1081;"
