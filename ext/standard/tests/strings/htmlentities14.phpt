--TEST--
htmlentities() test 14 (default_charset / Shift_JIS)
--INI--
output_handler=
mbstring.internal_encoding=pass
default_charset=Shift_JIS
--FILE--
<?php
	print ini_get('default_charset')."\n";
	var_dump(htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, ''));
?>
--EXPECT--
Shift_JIS
string(6) "ÅAÅBÅC"
