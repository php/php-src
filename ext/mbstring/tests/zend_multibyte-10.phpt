--TEST--
zend multibyte (10)
--SKIPIF--
<?php require 'skipif.inc'; ?>
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
