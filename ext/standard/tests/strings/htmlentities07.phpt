--TEST--
htmlentities() test 7 (mbstring / ISO-8859-1)
--INI--
output_handler=
mbstring.internal_encoding=ISO-8859-1
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
--FILE--
<?php
	mb_internal_encoding('ISO-8859-1');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xe4\xf6\xfc", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-1
string(18) "&auml;&ouml;&uuml;"
