--TEST--
Ensure that DOMImplementationList::item triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$d = new DOMImplementationList();
echo $d->item(1);
?>
--EXPECTF--	
Warning: DOMImplementationList::item(): Not yet implemented in %s on line %d