--TEST--
zend multibyte (10)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="ISO-8859-15");
declare(encoding="ISO-8859-1");
echo "ok\n";
?>
--EXPECT--
ok
