--TEST--
Checks that DOMNameList::length is still under development.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$foo = new DOMNameList();
echo $foo->length;
?>
--EXPECTF--	
TEST