--TEST--
htmlentities() test 2 (setlocale / fr_FR.ISO-8859-15) 
--SKIPIF--
<?php setlocale(LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15") or die("skip setlocale() failed\n"); ?>
--INI--
mbstring.internal_encoding=pass
--FILE--
<?php
	setlocale( LC_CTYPE, "fr_FR.ISO-8859-15", "fr_FR.ISO8859-15" );
	var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
string(20) "&OElig;&oelig;&Yuml;"
