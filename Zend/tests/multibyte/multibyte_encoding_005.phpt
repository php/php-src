--TEST--
encoding conversion from script encoding into internal encoding
--SKIPIF--
<?php
if (!in_array("zend.detect_unicode", array_keys(ini_get_all()))) {
  die("skip Requires configure --enable-zend-multibyte option");
}
if (!extension_loaded("mbstring")) {
  die("skip Requires mbstring extension");
}
?>
--INI--
zend.multibyte=1
mbstring.encoding_translation = On
zend.script_encoding=Shift_JIS
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
