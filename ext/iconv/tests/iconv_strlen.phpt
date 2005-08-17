--TEST--
iconv_strlen()
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--INI--
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
--FILE--
<?php
function foo($str, $charset) {
	var_dump(strlen($str));
	var_dump(iconv_strlen($str, $charset));
}

foo("abc", "ASCII");
foo("ÆüËÜ¸ì EUC-JP", "EUC-JP");
?>
--EXPECT--
int(3)
int(3)
int(13)
int(10)
