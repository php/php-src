--TEST--
htmlentities() test 7 (mbstring / ISO-8859-1)
--INI--
output_handler=
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
	mb_internal_encoding('ISO-8859-1');
	$php_errormsg = NULL;
	@htmlentities("\xe4\xf6\xfc", ENT_QUOTES, '');
	if ($php_errormsg) {
		die("skip ISO-8859-1 chracter set is not supported on this platform.\n");
	}
?>
--FILE--
<?php
	mb_internal_encoding('ISO-8859-1');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xe4\xf6\xfc", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-1
string(18) "&auml;&ouml;&uuml;"
