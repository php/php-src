--TEST--
Ensure that DOMNamedNodeMap::setNamedItem triggers warning 'Not yet implemented'.
--CREDITS--
PHP TestFest 2017 - PHPDublin - Sheldon Led <sheldonled@gmail.com>
--FILE--
<?php
$nm = new DOMNamedNodeMap();
$nm->setNamedItem(new DOMNode());
?>
--EXPECTF--	
Warning: DOMNamedNodeMap::setNamedItem(): Not yet implemented in %s on line %d