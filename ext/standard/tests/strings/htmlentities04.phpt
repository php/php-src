--TEST--
htmlentities() test 4 (setlocale / ja_JP.EUC-JP)
--SKIPIF--
<?php
$result = (bool)setlocale(LC_CTYPE, "ja_JP.EUC-JP", "ja_JP.eucJP");
if (!$result || preg_match('/EUC[^a-zA-Z]*JP/i', setlocale(LC_CTYPE, 0)) == 0) {
	die("skip setlocale() failed\n");
}
?>
--INI--
output_handler=
default_charset=
internal_encoding=pass
--FILE--
<?php
    setlocale( LC_CTYPE, "ja_JP.EUC-JP", "ja_JP.eucJP" );
    var_dump(htmlentities("\xa1\xa2\xa1\xa3\xa1\xa4", ENT_QUOTES, ''));
?>
--EXPECTF--
Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(6) "������"
