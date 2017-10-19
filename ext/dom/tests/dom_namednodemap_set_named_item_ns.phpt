--TEST--
Ensure that DOMNamedNodeMap::setNamedItemNS triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$nm = new DOMNamedNodeMap();
$nm->setNamedItemNS(new DOMNode());
?>
--EXPECTF--	
Warning: DOMNamedNodeMap::setNamedItemNS(): Not yet implemented in %s on line %d