--TEST--
htmlentities() test 6 (mbstring / ISO-8859-15)
--INI--
output_handler=
--SKIPIF--
<?php
if (!extension_loaded("mbstring") && ini_get("enable_dl")) {
	$dlext = (substr(PHP_OS, 0, 3) == "WIN" ? ".dll" : ".so");
	@dl("mbstring$dlext");
}
extension_loaded("mbstring") or die("skip mbstring not available\n");
?>
--FILE--
<?php
	if (!extension_loaded("mbstring") && ini_get("enable_dl")) {
		$dlext = (substr(PHP_OS, 0, 3) == "WIN" ? ".dll" : ".so");
		@dl("mbstring$dlext");
	}
	mb_internal_encoding('ISO-8859-15');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\xbc\xbd\xbe", ENT_QUOTES, ''));
?>
--EXPECT--
ISO-8859-15
string(20) "&OElig;&oelig;&Yuml;"
