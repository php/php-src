--TEST--
htmlentities() test 13 (default_charset / EUC-JP)
--INI--
output_handler=
mbstring.internal_encoding=pass
default_charset=EUC-JP
--FILE--
<?php
	print ini_get('default_charset')."\n";
	var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECT--
EUC-JP
string(6) "¡¢¡£¡¤"
