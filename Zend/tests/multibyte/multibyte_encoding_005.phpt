--TEST--
encoding conversion from script encoding into internal encoding
--SKIPIF--
<?php
ini_set("mbstring.script_encoding","SJIS");
if (ini_set("mbstring.script_encoding","SJIS") != "SJIS") {
	die("skip zend-multibyte is not available");
}
?>
--INI--
mbstring.encoding_translation = On
mbstring.script_encoding=Shift_JIS
mbstring.internal_encoding=UTF-8
--FILE--
<?php
	function —\Ž\”\($ˆø”)
	{
		echo $ˆø”;
	}

	—\Ž\”\("ƒhƒŒƒ~ƒtƒ@ƒ\");
?>
--EXPECT--
ãƒ‰ãƒ¬ãƒŸãƒ•ã‚¡ã‚½
