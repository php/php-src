--TEST--
htmlentities() test 16 (mbstring / cp1251)
--INI--
output_handler=
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
	mb_internal_encoding('cp1251');
	$php_errormsg = NULL;
	@htmlentities("\x88\xa9\xd2\xcf\xd3\xcb\xcf\xdb\xce\xd9\xca", ENT_QUOTES, '');
	if ($php_errormsg) {
		die("skip cp1251 chracter set is not supported on this platform.\n");
	}
?>
--FILE--
<?php
mb_internal_encoding('cp1251');
$str = "\x88\xa9\xf0\xee\xf1\xea\xee\xf8\xed\xfb\xe9";
var_dump($str, htmlentities($str, ENT_QUOTES, ''));
?>
--EXPECT--
string(11) "┬╘роскошный"
string(63) "&euro;&copy;&#1088;&#1086;&#1089;&#1082;&#1086;&#1096;&#1085;&#1099;&#1081;"
