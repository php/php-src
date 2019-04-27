--TEST--
zend multibyte (11)
--SKIPIF--
<?php require 'skipif.inc'; ?>
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
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line %d
