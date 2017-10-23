--TEST--
Ensure that DOMImplementationSource::getDomimplementation triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$foo = new DOMImplementationSource();
$foo->getDomimplementation();
?>
--EXPECTF--	
Warning: DOMImplementationSource::getDomimplementation(): Not yet implemented in %s on line %d