--TEST--
htmlentities() test 2 (setlocale / fr_FR.ISO-8859-15) 
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15");
if (!$result || preg_match('/ISO/i', setlocale(LC_CTYPE, 0)) == 0) {
	die("skip setlocale() failed\n");
}
?>
--INI--
output_handler=
mbstring.internal_encoding=pass
--FILE--
<?php
	setlocale( LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15" );
	var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
string(20) "&OElig;&oelig;&Yuml;"
