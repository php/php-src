--TEST--
htmlentities() test 8 (mbstring / EUC-JP)
--INI--
output_handler=
mbstring.internal_encoding=EUC-JP
--SKIPIF--
<?php function_exists('mb_internal_encoding') or die("skip\n"); ?>
--FILE--
<?php
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECT--
EUC-JP
string(6) "¡¢¡£¡¤"
