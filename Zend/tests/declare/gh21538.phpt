--TEST--
GH-21538 (declare(encoding=...) warning with zend.multibyte=0 should be skipped for UTF-8 and ASCII)
--INI--
zend.multibyte=0
--FILE--
<?php
eval("declare(encoding='UTF-8');");
eval("declare(encoding='ASCII');");
eval("declare(encoding='ISO-8859-1');");
echo "Done\n";
?>
--EXPECTF--
Warning: declare(encoding=...) ignored because Zend multibyte feature is turned off by settings in %s on line %d
Done
