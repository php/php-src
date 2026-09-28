--TEST--
zend multibyte (11)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="ISO-8859-15") {
    declare(encoding="ISO-8859-1");
    echo "ok\n";
}
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must not use block mode in %s on line %d
