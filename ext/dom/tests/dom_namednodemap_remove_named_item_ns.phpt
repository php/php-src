--TEST--
Ensure that DOMNamedNodeMap::removeNamedItemNS triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$nm = new DOMNamedNodeMap();
$nm->removeNamedItemNS();
?>
--EXPECTF--	
Warning: DOMNamedNodeMap::removeNamedItemNS(): Not yet implemented in %s on line %d