--TEST--
htmlentities() test 9 (mbstring / Shift_JIS)
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
	mb_internal_encoding('Shift_JIS');
	print mb_internal_encoding()."\n";
	var_dump(htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, ''));
?>
--EXPECT--
SJIS
string(6) "ÅAÅBÅC"
