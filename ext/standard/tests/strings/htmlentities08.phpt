--TEST--
htmlentities() test 8 (mbstring / EUC-JP)
--INI--
output_handler=
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
	mb_internal_encoding('EUC-JP');
	$php_errormsg = NULL;
	@htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, '');
	if ($php_errormsg) {
		die("skip EUC-JP chracter set is not supported on this platform.\n");	
	}
?>
--FILE--
<?php
	mb_internal_encoding('EUC-JP');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECT--
EUC-JP
string(6) "¡¢¡£¡¤"
