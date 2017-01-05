--TEST--
Bug #69092-2 (Declare Encoding Compile Check Wrong) - multibyte off
--INI--
zend.multibyte=0
--FILE--
<?php
echo "Hi";

function foo() {
	declare(encoding="UTF-8");
}

echo "Bye"
?>
--EXPECTF--
Warning: declare(encoding=...) ignored because Zend multibyte feature is turned off by settings in %s on line %d

Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line %d