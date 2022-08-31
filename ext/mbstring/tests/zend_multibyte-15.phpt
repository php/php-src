--TEST--
zend multibyte (15)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding="ISO-8859-15") {
    echo "ok\n";
}
declare(encoding="UTF-8") {
    echo "ok\n";
}
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must not use block mode in %s on line %d
