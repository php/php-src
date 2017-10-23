--TEST--
Ensure that DOMNamedNodeMap::removeNamedItem triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$nm = new DOMNamedNodeMap();
$nm->removeNamedItem();
?>
--EXPECTF--	
Warning: DOMNamedNodeMap::removeNamedItem(): Not yet implemented in %s on line %d