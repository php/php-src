--TEST--
zend multibyte (10)
--SKIPIF--
--XFAIL--
https://bugs.php.net/bug.php?id=66582 - still leaks memory which causes fail in debug mode
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="ISO-8859-15");
declare(encoding="ISO-8859-1");
echo "ok\n";
?>
--EXPECTF--
ok
