--TEST--
htmlentities() test 3 (setlocale / de_DE.ISO-8859-1)
--SKIPIF--
<?php setlocale(LC_CTYPE, "de_DE.ISO-8859-1") or die("skip setlocale() failed\n"); ?>
--INI--
mbstring.internal_encoding=pass
--FILE--
<?php
	setlocale( LC_CTYPE, "de_DE.ISO-8859-1" );
	var_dump(htmlentities("\xe4\xf6\xfc", ENT_QUOTES, ''));
?>
--EXPECT--
string(18) "&auml;&ouml;&uuml;"
