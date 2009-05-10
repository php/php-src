--TEST--
zend multibyte (1)
--SKIPIF--
<?php
ini_set("mbstring.script_encoding","SJIS");
if (ini_set("mbstring.script_encoding","SJIS") != "SJIS") {
	die("skip zend-multibyte is not available");
}
?>
--INI--
mbstring.script_encoding=Shift_JIS
mbstring.internal_encoding=Shift_JIS
--FILE--
<?php
	function 予蚕能($引数)
	{
		echo $引数;
	}

	予蚕能("ドレミファソ");
?>
--EXPECT--
ドレミファソ
