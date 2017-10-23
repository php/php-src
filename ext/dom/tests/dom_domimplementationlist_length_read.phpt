--TEST--
Checks that DOMImplementationList::length is still under development.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$d = new DOMImplementationList();
echo $d->length;
?>
--EXPECTF--	
TEST