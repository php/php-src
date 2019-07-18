--TEST--
htmlentities() test 9 (mbstring / Shift_JIS)
--INI--
output_handler=
internal_encoding=Shift_JIS
--SKIPIF--
<?php
	extension_loaded("mbstring") or die("skip mbstring not available\n");
--FILE--
<?php
	mb_internal_encoding('Shift_JIS');
	print mb_internal_encoding()."\n";
	var_dump(bin2hex(htmlentities("\x81\x41\x81\x42\x81\x43", ENT_QUOTES, '')));
?>
===DONE===
--EXPECTF--
SJIS

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(12) "814181428143"
===DONE===
