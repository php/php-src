--TEST--
htmlentities() test 6 (mbstring / ISO-8859-15)
--INI--
output_handler=
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
	mb_internal_encoding('ISO-8859-15');
	@htmlentities("\xbc\xbd\xbe", ENT_QUOTES, '');
	if ($php_errormsg) {
		die("skip ISO-8859-15 chracter set is not supported on this platform.\n");
	}
?>
--FILE--
<?php
	mb_internal_encoding('ISO-8859-15');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-15
string(20) "&OElig;&oelig;&Yuml;"
