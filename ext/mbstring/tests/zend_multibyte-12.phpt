--TEST--
zend multibyte (12)
--SKIPIF--
--XFAIL--
https://bugs.php.net/bug.php?id=66582
--FILE--
<?php
declare(encoding="ISO-8859-15");
echo 1;
declare(encoding="ISO-8859-1");
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line 4
