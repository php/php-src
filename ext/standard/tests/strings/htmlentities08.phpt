--TEST--
htmlentities() test 8 (mbstring / EUC-JP)
--INI--
output_handler=
--SKIPIF--
<?php
/*
if (!extension_loaded("mbstring") && ini_get("enable_dl")) {
	$dlext = (substr(PHP_OS, 0, 3) == "WIN" ? ".dll" : ".so");
	@dl("mbstring$dlext");
}
*/
extension_loaded("mbstring") or die("skip mbstring not available\n");
?>
--FILE--
<?php
/*
	if (!extension_loaded("mbstring") && ini_get("enable_dl")) {
		$dlext = (substr(PHP_OS, 0, 3) == "WIN" ? ".dll" : ".so");
		@dl("mbstring$dlext");
	}
*/
	mb_internal_encoding('EUC-JP');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECT--
EUC-JP
string(6) "¡¢¡£¡¤"
