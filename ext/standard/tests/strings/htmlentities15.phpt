--TEST--
htmlentities() test 15 (setlocale / KOI8-R)
--INI--
output_handler=
default_charset=
mbstring.internal_encoding=none
unicode.script_encoding=KOI-8
unicode.output_encoding=KOI-8
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "ru_RU.koi8r", "ru_RU.KOI8-R");
if (!$result || preg_match('/koi8/i', setlocale(LC_CTYPE, 0)) == 0) {
	die("skip setlocale() failed\n");
}
echo "warn possibly braindead libc\n";
?>
--FILE--
<?php
setlocale(LC_CTYPE, "ru_RU.koi8r", "ru_RU.KOI8-R");
$str = "роскошный";
var_dump($str, htmlentities($str, ENT_QUOTES, ''));
?>
--EXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
unicode(9) "роскошный"
unicode(9) "роскошный"
