--TEST--
htmlentities() test 9 (mbstring / Shift_JIS)
--INI--
output_handler=
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
	mb_internal_encoding('Shift_JIS');
	$php_errormsg = NULL;
	@htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, '');
	if ($php_errormsg) {
		die("skip Shift_JIS chracter set is not supported on this platform.\n");
	}
?>
--FILE--
<?php
	mb_internal_encoding('Shift_JIS');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, ''));
?>
--EXPECT--
SJIS
string(6) "ÅAÅBÅC"
