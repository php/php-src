--TEST--
htmlentities() test 10 (default_charset / cp1252)
--INI--
output_handler=
mbstring.internal_encoding=pass
default_charset=cp1252
--FILE--
<?php
	print ini_get('default_charset')."\n";
	var_dump(htmlentities("\x82\x86\x99\x9f", ENT_QUOTES, ''));
	var_dump(htmlentities("\x80\xa2\xa3\xa4\xa5", ENT_QUOTES, ''));
?>
--EXPECT--
cp1252
string(28) "&sbquo;&dagger;&trade;&Yuml;"
string(32) "&euro;&cent;&pound;&curren;&yen;"
