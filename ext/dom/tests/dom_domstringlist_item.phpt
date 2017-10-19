--TEST--
Ensure that DOMStringList::item triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$foo = new DOMStringList();
$foo->item();
?>
--EXPECTF--	
Warning: DOMStringList::item(): Not yet implemented in %s on line %d