--TEST--
zend multibyte (12)
--SKIPIF--
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="ISO-8859-15");
echo 1;
declare(encoding="ISO-8859-1");
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line 4
