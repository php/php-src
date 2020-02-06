--TEST--
zend multibyte (15)
--SKIPIF--
<?php require 'skipif.inc'; ?>
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
--EXPECT--
ok
ok
