--TEST--
Checks that DOMStringList::length is still under development.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$foo = new DOMStringList();
echo $foo->length;
?>
--EXPECTF--	
TEST