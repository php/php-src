--TEST--
Bug #69092 (Declare Encoding Compile Check Wrong)
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=On
--FILE--
<?php
echo "Hi";

function foo() {
    declare(encoding="utf-8");
}

echo "Bye"
?>
--EXPECTF--
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line %d
