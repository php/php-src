--TEST--
htmlentities() test 5 (mbstring / cp1252)
--INI--
output_handler=
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
	mb_internal_encoding('cp1252');
	$php_errormsg = NULL;
	@htmlentities("\x82\x86\x99\x9f", ENT_QUOTES, '');
	if ($php_errormsg) {
		die("skip cp1252 chracter set is not supported on this platform.\n");
	}
?>
--FILE--
<?php
	mb_internal_encoding('cp1252');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\x82\x86\x99\x9f", ENT_QUOTES, ''));
	var_dump(htmlentities("\x80\xa2\xa3\xa4\xa5", ENT_QUOTES, ''));
?>
--EXPECT--
Windows-1252
string(28) "&sbquo;&dagger;&trade;&Yuml;"
string(32) "&euro;&cent;&pound;&curren;&yen;"
