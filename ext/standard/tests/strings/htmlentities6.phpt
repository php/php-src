--TEST--
htmlentities() test 6 (mbstring / ISO-8859-15)
--INI--
output_handler=
mbstring.internal_encoding=ISO-8859-15
--SKIPIF--
<?php function_exists('mb_internal_encoding') or die("skip\n"); ?>
--FILE--
<?php
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-15
string(20) "&OElig;&oelig;&Yuml;"
