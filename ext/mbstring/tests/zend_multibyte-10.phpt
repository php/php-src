--TEST--
zend multibyte (10)
--SKIP--
<?php
ini_set("mbstring.script_encoding", "SJIS");
ini_set("mbstring.script_encoding", "***") != "SJIS" or
	die("skip zend-multibyte is not available");
?>
--FILE--
<?php
declare(encoding="ISO-8859-15");
declare(encoding="ISO-8859-1");
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line 3
