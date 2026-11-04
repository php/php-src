--TEST--
zend multibyte encoding declare cannot use block mode
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="ISO-8859-15"):
    echo "ok\n";
enddeclare;
?>
--EXPECTF--
Fatal error: Encoding declaration must not use block mode in %s on line %d
