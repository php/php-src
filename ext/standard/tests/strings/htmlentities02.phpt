--TEST--
htmlentities() test 2 (setlocale / fr_FR.ISO-8859-15)
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15", 'fr_FR@euro');
if (!$result) {
    die("skip setlocale() failed\n");
}
?>
--INI--
output_handler=
default_charset=
internal_encoding=
--FILE--
<?php
// Locale-based encoding guessing no longer works.
setlocale(LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15", 'fr_FR@euro');
var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
string(0) ""
