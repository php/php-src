--TEST--
htmlentities() test 1 (cp1252)
--INI--
output_handler=
mbstring.internal_encoding=pass
--FILE--
<?php
	var_dump(htmlentities(b"\x82\x86\x99\x9f", ENT_QUOTES, 'cp1252'));
	var_dump(htmlentities(b"\x80\xa2\xa3\xa4\xa5", ENT_QUOTES, 'cp1252'));
?>
--EXPECT--
string(28) "&sbquo;&dagger;&trade;&Yuml;"
string(32) "&euro;&cent;&pound;&curren;&yen;"
