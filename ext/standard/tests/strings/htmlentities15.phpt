--TEST--
htmlentities() test 15 (setlocale / KOI8-R)
--INI--
output_handler=
default_charset=
internal_encoding=pass
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "ru_RU.koi8r", "ru_RU.KOI8-R");
if (!$result || preg_match('/koi8/i', setlocale(LC_CTYPE, 0)) == 0) {
	die("skip setlocale() failed\n");
}
?>
--DESCRIPTION--
As of PHP 5.4, htmlentities() no longer makes replacements with numerical
entities. Hence, for this input there's no substitution.
--FILE--
<?php
setlocale(LC_CTYPE, "ru_RU.koi8r", "ru_RU.KOI8-R");
$str = "роскошный";
var_dump($str, htmlentities($str, ENT_QUOTES, ''));
?>
--EXPECT--
string(9) "роскошный"
string(9) "роскошный"
