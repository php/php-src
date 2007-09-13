--TEST--
htmlentities() test 4 (setlocale / ja_JP.EUC-JP)
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "ja_JP.EUC-JP", "ja_JP.eucJP");
if (!$result || preg_match('/EUC[^a-zA-Z]*JP/i', setlocale(LC_CTYPE, 0)) == 0) {
	die("skip setlocale() failed\n");
}
echo "warn possibly braindead libc\n";
?>
--INI--
output_handler=
default_charset=
mbstring.internal_encoding=none
--FILE--
<?php
	setlocale( LC_CTYPE, "ja_JP.EUC-JP", "ja_JP.eucJP" );
	var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECT--
string(6) "¡¢¡£¡¤"
