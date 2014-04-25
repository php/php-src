--TEST--
zend multibyte (11)
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
ok
