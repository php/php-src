--TEST--
htmlentities() test 9 (mbstring / Shift_JIS)
--INI--
output_handler=
mbstring.internal_encoding=Shift_JIS
--SKIPIF--
<?php function_exists('mb_internal_encoding') or die("skip\n"); ?>
--FILE--
<?php
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, ''));
?>
--EXPECT--
SJIS
string(6) "ÅAÅBÅC"
