--TEST--
htmlentities() test 4 (setlocale / ja_JP.EUC-JP)
--SKIPIF--
<?php setlocale( LC_CTYPE, "ja_JP.EUC-JP" ) or die("skip\n"); ?>
--INI--
mbstring.internal_encoding=pass
--FILE--
<?php
	setlocale( LC_CTYPE, "ja_JP.EUC-JP" );
	var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECT--
string(6) "¡¢¡£¡¤"
