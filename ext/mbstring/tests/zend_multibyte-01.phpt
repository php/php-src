--TEST--
zend multibyte (1)
--SKIPIF--
--INI--
zend.multibyte=On
zend.script_encoding=Shift_JIS
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
