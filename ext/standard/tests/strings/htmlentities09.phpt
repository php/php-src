--TEST--
htmlentities() test 9 (mbstring / Shift_JIS)
--INI--
output_handler=
error_reporting=~E_STRICT
mbstring.internal_encoding=Shift_JIS
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
--FILE--
<?php
	mb_internal_encoding('Shift_JIS');
	print mb_internal_encoding()."\n";
	var_dump(bin2hex(htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, '')));
?>
===DONE===
--EXPECT--
SJIS
string(12) "814181428143"
===DONE===
