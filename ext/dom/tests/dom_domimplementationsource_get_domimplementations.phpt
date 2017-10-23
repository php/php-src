--TEST--
Ensure that DOMImplementationSource::getDomimplementations triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$foo = new DOMImplementationSource();
$foo->getDomimplementations();
?>
--EXPECTF--	
Warning: DOMImplementationSource::getDomimplementations(): Not yet implemented in %s on line %d