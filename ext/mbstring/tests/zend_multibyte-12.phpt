--TEST--
zend multibyte (12)
--SKIPIF--
<?php
ini_set("mbstring.script_encoding","SJIS");
if (ini_set("mbstring.script_encoding","SJIS") != "SJIS") {
	die("skip zend-multibyte is not available");
}
?>
--FILE--
<?php
declare(encoding="ISO-8859-15");
echo 1;
declare(encoding="ISO-8859-1");
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line 4
