--TEST--
htmlentities() test 2 (setlocale / fr_FR.ISO-8859-15) 
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15", 'fr_FR@euro');
if (!$result) {
	die("skip setlocale() failed\n");
}
echo "warn possibly braindead libc\n";
?>
--INI--
output_handler=
default_charset=
mbstring.internal_encoding=none
--FILE--
<?php
	setlocale(LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15", 'fr_FR@euro');
	var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
string(20) "&OElig;&oelig;&Yuml;"
